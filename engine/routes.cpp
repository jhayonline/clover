#include "../include/routes.hpp"
#include "../include/router.hpp"
#include <iostream>

Routes &Routes::prefix(const std::string &p) {
  prefix_ = p;
  return *this;
}

Routes &Routes::add(const std::string &path, const std::string &method,
                    std::function<Response(Request)> handler) {
  routes_.push_back({path, method, handler});
  return *this;
}

Routes &Routes::get(const std::string &path,
                    std::function<Response(Request)> handler) {
  return add(path, "GET", handler);
}

Routes &Routes::post(const std::string &path,
                     std::function<Response(Request)> handler) {
  return add(path, "POST", handler);
}

Routes &Routes::put(const std::string &path,
                    std::function<Response(Request)> handler) {
  return add(path, "PUT", handler);
}

Routes &Routes::del(const std::string &path,
                    std::function<Response(Request)> handler) {
  return add(path, "DELETE", handler);
}

void Routes::mount(Router &router) const {
  std::cout << "Mounting " << routes_.size() << " routes..." << std::endl;
  for (const auto &route : routes_) {
    std::string full_path = prefix_ + route.path;
    router.add_route(route.method, full_path, route.handler);
    std::cout << "Route registered: " << route.method << " " << full_path
              << std::endl;
  }
}
