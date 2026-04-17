#pragma once
#include "request.hpp"
#include "response.hpp"
#include <functional>
#include <vector>

using Next = std::function<Response()>;
using MiddlewareFunc = std::function<Response(Request &, Next)>;

class Middleware {
public:
  void use(MiddlewareFunc middleware);
  Response run(Request &req, std::function<Response(Request &)> handler);

private:
  std::vector<MiddlewareFunc> middlewares_;
};
