#pragma once
#include "request.hpp"
#include "response.hpp"
#include <functional>
#include <string>
#include <vector>

// Forward declaration
class Router;

class Routes {
public:
  Routes &prefix(const std::string &p);
  Routes &add(const std::string &path, const std::string &method,
              std::function<Response(Request)> handler);

  // Convenience methods for different HTTP methods
  Routes &get(const std::string &path,
              std::function<Response(Request)> handler);
  Routes &post(const std::string &path,
               std::function<Response(Request)> handler);
  Routes &put(const std::string &path,
              std::function<Response(Request)> handler);
  Routes &del(const std::string &path,
              std::function<Response(Request)> handler);

  void mount(Router &router) const;

private:
  struct Route {
    std::string path;
    std::string method;
    std::function<Response(Request)> handler;
  };

  std::string prefix_;
  std::vector<Route> routes_;
};
