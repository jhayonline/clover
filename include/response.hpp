#pragma once
#include <string>
#include <unordered_map>

class Response {
public:
  int status_code = 200;
  std::unordered_map<std::string, std::string> headers;
  std::string body;

  Response() = default;
  Response(int status, const std::string &body);

  // String conversion constructor (for auto-conversion)
  Response(const std::string &body); // Implicit conversion from string

  std::string to_string() const;

  // Helper methods
  void set_header(const std::string &key, const std::string &value);
  std::string get_header(const std::string &key) const;

  // JSON helper
  static Response json(const std::string &json_body, int status = 200);
  static Response json(int status, const std::string &json_body);
};
