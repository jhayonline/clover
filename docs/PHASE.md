### Phase 0: Project Setup
- [ ] Basic CMake configuration
- [ ] Empty header files with basic struct definitions
- [ ] Simple "hello world" that compiles

### Phase 1: Core HTTP Foundation
**Goal:** Accept a connection and send a response

- [ ] **Basic socket server** - Listen on port, accept one connection
- [ ] **Parse simple GET request** - Just method and path (no headers, no body)
- [ ] **Send hardcoded response** - "HTTP/1.1 200 OK\r\n\r\nHello"
- [ ] **Close connection** - One request = one response = close

### Phase 2: Request/Response Types
**Goal:** Structured request/response with basic HTTP/1.1

- [ ] **Request struct** - method, path, version, headers map
- [ ] **Response struct** - status_code, headers map, body
- [ ] **Parse headers** - Read all headers into map
- [ ] **Parse body** - Read Content-Length bytes after headers
- [ ] **Response to string** - Serialize Response back to HTTP format

### Phase 3: Simple Routing
**Goal:** Map paths to handler functions

- [ ] **Router class** - `std::unordered_map<std::string, Handler>` for exact matches
- [ ] **Handler type** - `std::function<Response(Request)>`
- [ ] **GET only** - No method differentiation yet
- [ ] **404 handling** - Return 404 for unregistered paths

### Phase 4: REST Routing & Response Helpers
**Goal:** REST-style routing and ergonomic responses

- [ ] **Method-aware routing** - Map key = "GET:/users", "POST:/users"
- [ ] **String auto-conversion** - Handler returning `std::string` becomes 200/text
- [ ] **Basic Response builder** - `Response::json()` static helper
- [ ] **Path parameters** - `/users/:id` → extract `id` into Request params

### Phase 5: Controller System
**Goal:** Organize handlers into classes like Loco

- [ ] **Routes builder pattern** - `Routes().prefix("/api").add("/", "GET", greet)`
- [ ] **Multiple handlers per controller** - Group related endpoints
- [ ] **Controller mounting** - `router.mount(HelloController::routes())`
- [ ] **No base class required** - Just convention-based design

### Phase 6: Struct-based Models with TinyORM
**Goal:** Database integration with Active Record pattern on structs

- [ ] **TinyORM dependency setup** - Add TinyORM to CMake
- [ ] **Model struct definition** - Plain structs with TinyORM macros/decorators
- [ ] **Static finder methods on structs** - `User::find(id)`, `User::all()`, `User::where()`
- [ ] **Table mapping** - Struct fields map to database columns
- [ ] **JSON serialization from struct** - Convert struct instance to Response::json()
- [ ] **CRUD operations** - `user.save()`, `user.update()`, `user.delete()`

**Example after Phase 6:**
```cpp
// models/User.hpp
struct User : TinyORM::Model<User> {
    int id;
    std::string email;
    std::string name;
    bool is_active;
    
    // Table name (optional, defaults to plural of struct name)
    static constexpr const char* table = "users";
};

// In controller
static Response show(Request req) {
    int id = std::stoi(req.params["id"]);
    auto user = User::find(id);
    if (user) {
        return Response::json(user->toJson());  // Converts struct to JSON
    }
    return Response::json({{"error", "Not found"}}, 404);
}

static Response create(Request req) {
    User user;
    user.name = "John";
    user.email = "john@example.com";
    user.is_active = true;
    user.save();  // INSERT into database
    return Response::json(user.toJson(), 201);
}
```

### Phase 7: Server Improvements
**Goal:** Handle multiple requests efficiently

- [ ] **Connection keep-alive** - Support multiple requests per connection
- [ ] **Thread pool** - `std::thread` pool for concurrent request handling
- [ ] **Database connection pooling** - TinyORM connection pool integration
- [ ] **Graceful shutdown** - SIGINT handling, clean socket/db cleanup
- [ ] **Static file serving** - Serve files from `./public` directory

### Phase 8: Middleware System (Days 17-19)
**Goal:** Request/response interception

- [ ] **Middleware type** - `std::function<Response(Request, Next)>`
- [ ] **Global middleware** - `server.use(middleware)`
- [ ] **Middleware chain** - Compose multiple middleware + handler
- [ ] **Short-circuiting** - Middleware can return without calling next
- [ ] **Route-specific middleware** - `router.use("/api", auth_middleware)`
- [ ] **Model-aware middleware** - Middleware can query struct models

### Phase 9: Developer Experience
**Goal:** Make it pleasant to use

- [ ] **Route groups with nesting** - `Routes().prefix("/api").group([&](auto& r) { ... })`
- [ ] **Query parameter parsing** - `?foo=bar&baz=qux`
- [ ] **Form data parsing** - `application/x-www-form-urlencoded`
- [ ] **Better error messages** - Parse errors with file/line context
- [ ] **Examples folder** - CRUD app with User struct model

### Phase 10: Advanced TinyORM Features
**Goal:** Full database functionality with struct models

- [ ] **Model relationships** - `User::hasMany<Post>()`, `Post::belongsTo<User>()`
- [ ] **Query builder** - `User::where("is_active", true)->orderBy("name")->get()`
- [ ] **Eager loading** - `User::with("posts")->find(id)`
- [ ] **Database transactions** - `DB::transaction([]() { ... })`
- [ ] **Schema builder** - Create/modify tables programmatically

### Phase 11: Advanced Features
- [ ] HTTPS support (OpenSSL)
- [ ] WebSockets
- [ ] Multipart file uploads
- [ ] Cookie parsing/setting
- [ ] Compression (gzip)
- [ ] HTTP/2
