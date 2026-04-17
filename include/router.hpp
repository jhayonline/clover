#pragma once
#include "middleware.hpp"
#include "request.hpp"
#include "response.hpp"
#include <functional>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

using Handler = std::function<Response(Request &)>;

class Router {
public:
  void add_route(const std::string &method, const std::string &path,
                 Handler handler);
  Response route(Request &req);

  // Mount routes from Routes builder
  void mount(const class Routes &routes);

  // Middleware support
  void use(MiddlewareFunc middleware);
  void use(const std::string &path_prefix, MiddlewareFunc middleware);

private:
  struct RouteEntry {
    Handler handler;
    std::regex path_regex;
    std::vector<std::string> param_names;
    std::vector<MiddlewareFunc> middlewares; // Route-specific middleware
  };

  struct MiddlewareEntry {
    std::string prefix;
    MiddlewareFunc func;
  };

  std::unordered_map<std::string, std::vector<RouteEntry>> routes_;
  std::vector<MiddlewareEntry> global_middlewares_;
  std::vector<MiddlewareEntry> route_middlewares_;

  Response apply_middlewares(Request &req,
                             const std::vector<MiddlewareFunc> &middlewares,
                             Handler handler);
};
