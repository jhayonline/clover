#include "../include/middleware.hpp"

void Middleware::use(MiddlewareFunc middleware) {
  middlewares_.push_back(middleware);
}

Response Middleware::run(Request &req,
                         std::function<Response(Request &)> handler) {
  // Build the chain from the end to the beginning
  std::function<Response()> chain = [&]() { return handler(req); };

  // Wrap from last middleware to first
  for (auto it = middlewares_.rbegin(); it != middlewares_.rend(); ++it) {
    auto middleware = *it;
    auto next = chain;
    chain = [&, middleware, next]() { return middleware(req, next); };
  }

  return chain();
}
