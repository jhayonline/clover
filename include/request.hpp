#pragma once
#include <string>
#include <unordered_map>

class Request {
public:
  std::string method;
  std::string path;
  std::string version;

  std::unordered_map<std::string, std::string> query;
  std::unordered_map<std::string, std::string> params;
  std::unordered_map<std::string, std::string> headers;

  std::string body;

  // Helper methods
  std::string get_query(const std::string &key) const;
  std::string get_header(const std::string &key) const;
};
