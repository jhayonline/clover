#include "../include/request.hpp"
#include <sstream>

std::string Request::get_query(const std::string &key,
                               const std::string &default_value) const {
  auto it = query.find(key);
  if (it != query.end()) {
    return it->second;
  }
  return default_value;
}

std::string Request::get_header(const std::string &key) const {
  auto it = headers.find(key);
  if (it != headers.end()) {
    return it->second;
  }
  return "";
}

nlohmann::json Request::json() const {
  if (body.empty()) {
    return nlohmann::json::object();
  }
  try {
    return nlohmann::json::parse(body);
  } catch (const std::exception &e) {
    std::cerr << "JSON parse error: " << e.what() << std::endl;
    return nlohmann::json::object();
  }
}

std::unordered_map<std::string, std::string> Request::form_data() const {
  std::unordered_map<std::string, std::string> result;

  // Check content type
  auto content_type = get_header("Content-Type");
  if (content_type.find("application/x-www-form-urlencoded") ==
      std::string::npos) {
    return result;
  }

  std::stringstream ss(body);
  std::string pair;
  while (std::getline(ss, pair, '&')) {
    size_t eq_pos = pair.find('=');
    if (eq_pos != std::string::npos) {
      std::string key = pair.substr(0, eq_pos);
      std::string value = pair.substr(eq_pos + 1);
      result[key] = value;
    }
  }

  return result;
}
