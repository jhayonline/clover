#include "../include/request.hpp"

std::string Request::get_query(const std::string &key) const {
  auto it = query.find(key);
  if (it != query.end()) {
    return it->second;
  }
  return "";
}

std::string Request::get_header(const std::string &key) const {
  auto it = headers.find(key);
  if (it != headers.end()) {
    return it->second;
  }
  return "";
}
