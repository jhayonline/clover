# 🍀 Clover

**Clover** is a lightweight C++ HTTP library for building fast and flexible backend systems.

It provides core building blocks—routing, request/response handling, middleware, and a server runtime, so you can build APIs and services without dealing with low-level networking. Clover is minimal and unopinionated, inspired by tools like Axum, but designed for the C++ ecosystem.

Clover does not enforce how you structure your code. You can organize your application however you like. Modular, MVC-style, or completely inline. It simply provides the HTTP layer and gets out of your way.

---

# Quick Disclaimer

Clover is a personal project I built for curiosity, not convention.

I wanted to understand what it really feels like to build a simple HTTP API in C++ from the ground up, without the overhead or complexity that comes with frameworks like Oatpp or Drogon. Don't get me wrong, they're both fantastic frameworks loved by a lot of C++ devs, but it's simply not for me. When I tried them, I didn’t enjoy how much setup and boilerplate it took just to get moving, so I built something intentionally minimal, something I could understand end-to-end and evolve on my own terms.

This is not production-ready and honestly I don't think it ever would, and it is not designed or optimized to be one.

It’s an experimental HTTP layer I built for learning, exploration, and fun. I might personally use it in real projects out of preference, but I don’t recommend it for production systems or serious production workloads.

Clover is me exploring C++ backend systems in the simplest way I know how. Nothing more, nothing less.

---

# Example Project Structure

Clover does not enforce structure, but a typical project might look like this:

```text
examples/
├── CMakeLists.txt
├── config/
│   ├── app.hpp
│   ├── env.hpp
│   ├── server.hpp
│
├── http/
│   ├── main.cpp
│   ├── app.cpp
│   ├── app.hpp
│   │
│   ├── controllers/
│   │   ├── base_controller.cpp
│   │   ├── hello_controller.cpp
│   │
│   ├── models/
│   │   └── user.hpp
│   │
│   ├── middlewares/
│   │   └── auth_middleware.hpp
│   │
│   └── utils/
│       └── json.hpp
```

This structure is **optional**—just one way to organize a growing codebase.

---

# Example

## Controller

```cpp
// src/hello_controller.cpp

#pragma once
#include <router.hpp>
#include <response.hpp>
#include <routes.hpp>

class HelloController {
public:
  static Response index(Request req) { return Response("Welcome to Clover!"); }

  static Response hello(Request req) {
    return Response::json(R"({"message": "Hello from Clover!"})");
  }

  static Routes routes() {
    return Routes()
      .prefix("/api")
      .get("/", index)
      .get("/hello", hello);
  }
};
```

---

## Model

```cpp
// user.hpp

#pragma once
#include <string>

struct User {
  int id;
  std::string pid;
  std::string email;
  std::string password;
  std::string name;
  bool is_active;

  static bool verify_password(const User &user, const std::string &password) {
    return user.password == password;
  }

  static User find_by_email(const std::string &email) {
    User user;
    user.email = email;
    return user;
  }
};
```

---

## App Routing
```cpp
// include/app.hpp

#pragma once
#include <router.hpp>

class App {
public:
  static Router routes();
};
```



```cpp
// src/app.cpp

#include "app.hpp"
#include "controllers/hello_controller.cpp"

Router App::routes() {
  Router router;

  router.mount(HelloController::routes());

  return router;
}
```

---

## Entry Point

```cpp
// src/main.cpp

#include <iostream>
#include <router.hpp>
#include <server.hpp>

int main() {
  std::cout << "🍀 Clover Server starting..." << std::endl;

  // Create and configure server
  Server server("0.0.0.0", 3000);
  
  // Attach router and start
  server.use(router);
  server.listen();

  return 0;
}
```

---

# Philosophy

Clover is not a framework.

It’s a focused HTTP layer that gives you:

* control over how your app is structured
* freedom to scale from simple to complex
* minimal abstractions without lock-in

You define the architecture. Clover handles the HTTP.

---

