#include "../include/response.hpp"
#include <string>

Response::Response(int status, const std::string &body)
    : status_code(status), body(body) {}

Response::Response(const std::string &body) : status_code(200), body(body) {}

std::string Response::to_string() const {
  std::string response;

  // Status line
  response += "HTTP/1.1 " + std::to_string(status_code) + " ";
  switch (status_code) {
  case 200:
    response += "OK";
    break;
  case 201:
    response += "Created";
    break;
  case 204:
    response += "No Content";
    break;
  case 400:
    response += "Bad Request";
    break;
  case 401:
    response += "Unauthorized";
    break;
  case 403:
    response += "Forbidden";
    break;
  case 404:
    response += "Not Found";
    break;
  case 500:
    response += "Internal Server Error";
    break;
  default:
    response += "Unknown";
  }
  response += "\r\n";

  // Auto-add Content-Length if not present and body exists
  std::string response_body = body;
  if (headers.find("Content-Length") == headers.end() &&
      !response_body.empty()) {
    const_cast<Response *>(this)->set_header(
        "Content-Length", std::to_string(response_body.size()));
  }

  // Headers
  for (const auto &[key, value] : headers) {
    response += key + ": " + value + "\r\n";
  }

  // End of headers
  response += "\r\n";

  // Body
  response += response_body;

  return response;
}

void Response::set_header(const std::string &key, const std::string &value) {
  headers[key] = value;
}

std::string Response::get_header(const std::string &key) const {
  auto it = headers.find(key);
  if (it != headers.end()) {
    return it->second;
  }
  return "";
}

Response Response::json(const std::string &json_body, int status) {
  Response res(status, json_body);
  res.set_header("Content-Type", "application/json");
  return res;
}

Response Response::json(int status, const std::string &json_body) {
  return json(json_body, status);
}
