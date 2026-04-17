#include "../include/server.hpp"
#include "../include/config.hpp"
#include "../include/request.hpp"
#include "../include/response.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>

Server::Server()
    : host_(CFG_STR("server.host", "0.0.0.0")),
      port_(CFG_INT("server.port", 3000)), server_fd_(-1), router_(nullptr) {}

Server::Server(const std::string &host, int port)
    : host_(host), port_(port), server_fd_(-1), router_(nullptr) {}

Server::~Server() {
  if (server_fd_ != -1) {
    close(server_fd_);
  }
}

void Server::use(Router &router) { router_ = &router; }

void Server::setup_socket() {
  server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd_ < 0) {
    std::cerr << "Failed to create socket" << std::endl;
    exit(1);
  }

  int opt = 1;
  if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    std::cerr << "Failed to set socket options" << std::endl;
    exit(1);
  }

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port_);

  if (bind(server_fd_, (struct sockaddr *)&address, sizeof(address)) < 0) {
    std::cerr << "Failed to bind to port " << port_ << std::endl;
    exit(1);
  }

  int backlog = CFG_INT("server.backlog", 3);
  if (::listen(server_fd_, backlog) < 0) {
    std::cerr << "Failed to listen on socket" << std::endl;
    exit(1);
  }

  std::cout << "Server listening on " << host_ << ":" << port_ << std::endl;
}

// In server.cpp - update parse_request() method

Request Server::parse_request(const std::string &raw_request) {
  Request req;

  // Parse request line
  std::istringstream stream(raw_request);
  std::string line;

  // First line: "GET /path?foo=bar&baz=qux HTTP/1.1"
  std::getline(stream, line);
  size_t first_space = line.find(' ');
  size_t second_space = line.find(' ', first_space + 1);

  if (first_space != std::string::npos && second_space != std::string::npos) {
    req.method = line.substr(0, first_space);
    std::string full_path =
        line.substr(first_space + 1, second_space - first_space - 1);
    req.version = line.substr(second_space + 1);

    if (!req.version.empty() && req.version.back() == '\r') {
      req.version.pop_back();
    }

    // Parse query parameters from path
    size_t query_pos = full_path.find('?');
    if (query_pos != std::string::npos) {
      req.path = full_path.substr(0, query_pos);
      std::string query_string = full_path.substr(query_pos + 1);

      // Parse key=value pairs
      std::stringstream query_stream(query_string);
      std::string pair;
      while (std::getline(query_stream, pair, '&')) {
        size_t eq_pos = pair.find('=');
        if (eq_pos != std::string::npos) {
          std::string key = pair.substr(0, eq_pos);
          std::string value = pair.substr(eq_pos + 1);
          // URL decode (simple version - replace %20 with space)
          size_t pos = 0;
          while ((pos = value.find('%', pos)) != std::string::npos) {
            if (pos + 2 < value.length()) {
              std::string hex = value.substr(pos + 1, 2);
              char decoded = static_cast<char>(std::stoi(hex, nullptr, 16));
              value.replace(pos, 3, 1, decoded);
            }
            pos++;
          }
          req.query[key] = value;
        }
      }
    } else {
      req.path = full_path;
    }
  }

  // Parse headers (existing code continues...)
  while (std::getline(stream, line) && line != "\r") {
    if (line.empty() || line == "\r")
      break;

    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }

    size_t colon_pos = line.find(':');
    if (colon_pos != std::string::npos) {
      std::string key = line.substr(0, colon_pos);
      std::string value = line.substr(colon_pos + 1);

      size_t start = value.find_first_not_of(" \t");
      if (start != std::string::npos) {
        value = value.substr(start);
      }

      req.headers[key] = value;
    }
  }

  // Parse body (existing code continues...)
  std::stringstream body_stream;
  body_stream << stream.rdbuf();
  req.body = body_stream.str();

  if (!req.body.empty() && req.body.back() == '\r') {
    req.body.pop_back();
  }

  std::cout << req.method << " " << req.path;
  if (!req.query.empty()) {
    std::cout << " [query: ";
    for (const auto &[k, v] : req.query) {
      std::cout << k << "=" << v << " ";
    }
    std::cout << "]";
  }
  std::cout << std::endl;

  return req;
}

void Server::handle_client(int client_fd) {
  char buffer[4096] = {0};

  int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
  if (bytes_read < 0) {
    std::cerr << "Failed to read from socket" << std::endl;
    close(client_fd);
    return;
  }

  // Parse request
  Request req = parse_request(std::string(buffer));

  // Route request
  Response res;
  if (router_) {
    res = router_->route(req);
  } else {
    res = Response(500, "No router configured");
    res.set_header("Content-Type", "text/plain");
  }

  // Send response
  std::string response_str = res.to_string();
  send(client_fd, response_str.c_str(), response_str.length(), 0);

  close(client_fd);
}

void Server::listen() {
  setup_socket();

  while (true) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_fd =
        accept(server_fd_, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) {
      std::cerr << "Failed to accept connection" << std::endl;
      continue;
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    std::cout << "New connection from " << client_ip << std::endl;

    handle_client(client_fd);
  }
}
