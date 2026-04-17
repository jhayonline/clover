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

# Installation

Use Clover with CMake:

```cmake
include(FetchContent)

FetchContent_Declare(
  clover
  GIT_REPOSITORY https://github.com/your-username/clover.git
  GIT_TAG main
)

FetchContent_MakeAvailable(clover)

add_executable(app main.cpp)

target_link_libraries(app PRIVATE clover)
```

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
#pragma once
#include "../../../include/router.hpp"

class HelloController {
public:
  static std::string greet(Request req) {
    return "Hello World";
  }

  static Routes routes() {
    return Routes()
      .prefix("/api/profile")
      .add("/", "GET", greet);
  }
};
```

---

## Model

```cpp
#pragma once
#include <string>

struct User {
  int id;
  std::string pid;
  std::string email;
  std::string password;
  std::string name;
  bool is_active;
};

class UserModel {
public:
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
#include "../../include/server.hpp"
#include "app.hpp"

int main() {
  Router router = App::routes();

  Server server("0.0.0.0", 3000);
  server.use(router);
  server.listen();
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

