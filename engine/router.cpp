#include "../include/router.hpp"
#include "../include/routes.hpp"
#include <iostream>
#include <regex>

void Router::add_route(const std::string &method, const std::string &path,
                       Handler handler) {
  // Convert path pattern to regex: /users/:id -> /users/([^/]+)
  std::string regex_pattern;
  std::vector<std::string> param_names;

  size_t i = 0;
  while (i < path.length()) {
    if (path[i] == ':' && i + 1 < path.length() && isalpha(path[i + 1])) {
      // Found a parameter
      size_t start = i + 1;
      while (i + 1 < path.length() &&
             (isalnum(path[i + 1]) || path[i + 1] == '_')) {
        i++;
      }
      std::string param_name = path.substr(start, i - start + 1);
      param_names.push_back(param_name);
      regex_pattern += "([^/]+)";
      i++;
    } else {
      // Regular character - escape if needed
      char c = path[i];
      if (c == '.' || c == '*' || c == '+' || c == '?' || c == '^' ||
          c == '$' || c == '(' || c == ')' || c == '[' || c == ']' ||
          c == '{' || c == '}' || c == '|' || c == '\\') {
        regex_pattern += '\\';
      }
      regex_pattern += c;
      i++;
    }
  }

  std::regex final_regex("^" + regex_pattern + "$");

  RouteEntry entry{handler, final_regex, param_names, {}};
  routes_[method].push_back(entry);

  std::cout << "Route registered: " << method << " " << path;
  std::cout << " -> regex: ^" << regex_pattern << "$";
  std::cout << " params: ";
  for (const auto &p : param_names)
    std::cout << p << " ";
  std::cout << std::endl;
}

void Router::use(MiddlewareFunc middleware) {
  global_middlewares_.push_back({"", middleware});
}

void Router::use(const std::string &path_prefix, MiddlewareFunc middleware) {
  route_middlewares_.push_back({path_prefix, middleware});
}

Response
Router::apply_middlewares(Request &req,
                          const std::vector<MiddlewareFunc> &middlewares,
                          Handler handler) {
  // Build chain
  std::function<Response()> chain = [&]() { return handler(req); };

  for (auto it = middlewares.rbegin(); it != middlewares.rend(); ++it) {
    auto middleware = *it;
    auto next = chain;
    chain = [&, middleware, next]() { return middleware(req, next); };
  }

  return chain();
}

Response Router::route(Request &req) {
  // Handle HEAD requests by using GET handler (without body)
  bool is_head = (req.method == "HEAD");
  std::string original_method = req.method;

  if (is_head) {
    req.method = "GET";
    std::cout << "Converting HEAD to GET for: " << req.path << std::endl;
  }

  std::cout << "Routing: " << req.method << " " << req.path << std::endl;

  // Collect matching route-specific middlewares
  std::vector<MiddlewareFunc> applicable_middlewares;

  // Add global middlewares first
  for (const auto &mw : global_middlewares_) {
    applicable_middlewares.push_back(mw.func);
  }

  // Add route-specific middlewares that match the path prefix
  for (const auto &mw : route_middlewares_) {
    if (req.path.find(mw.prefix) == 0) {
      applicable_middlewares.push_back(mw.func);
    }
  }

  Response res;
  auto it = routes_.find(req.method);
  if (it != routes_.end()) {
    // Try each route for this method
    for (const auto &route : it->second) {
      std::smatch matches;
      if (std::regex_match(req.path, matches, route.path_regex)) {
        // Extract path parameters
        req.params.clear();
        for (size_t i = 0;
             i < route.param_names.size() && i + 1 < matches.size(); ++i) {
          req.params[route.param_names[i]] = matches[i + 1];
          std::cout << "Param: " << route.param_names[i] << " = "
                    << matches[i + 1] << std::endl;
        }

        // Add route-specific middlewares from the route entry
        for (const auto &mw : route.middlewares) {
          applicable_middlewares.push_back(mw);
        }

        // Apply middlewares and call handler
        res = apply_middlewares(req, applicable_middlewares, route.handler);

        // Restore original method if this was a HEAD request
        if (is_head) {
          req.method = original_method;
          // Remove body for HEAD response
          res.body = "";
          res.set_header("Content-Length", "0");
        }

        return res;
      }
    }
  }

  std::cout << "No route found for: " << req.method << " " << req.path
            << std::endl;

  // 404 Not Found
  res = Response(404, "404 Not Found");
  res.set_header("Content-Type", "text/plain");

  if (is_head) {
    res.body = "";
    res.set_header("Content-Length", "0");
    req.method = original_method; // Restore original method
  }

  return res;
}

void Router::mount(const class Routes &routes) { routes.mount(*this); }
