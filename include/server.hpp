#pragma once
#include "request.hpp"
#include "router.hpp"
#include <string>

class Server {
public:
  Server();
  Server(const std::string &host, int port);
  ~Server();

  void listen();
  void use(Router &router);

private:
  std::string host_;
  int port_;
  int server_fd_;
  Router *router_;

  void setup_socket();
  void handle_client(int client_fd);
  Request parse_request(const std::string &raw_request);
};
