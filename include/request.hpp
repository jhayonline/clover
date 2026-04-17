#pragma once
#include <nlohmann/json.hpp>
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
  std::string get_query(const std::string &key,
                        const std::string &default_value = "") const;
  std::string get_header(const std::string &key) const;

  // JSON parsing
  nlohmann::json json() const;
  template <typename T> T json_as() const { return json().get<T>(); }

  // Form data parsing
  std::unordered_map<std::string, std::string> form_data() const;
};
