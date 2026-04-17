cmake_minimum_required(VERSION 3.16)
project(Clover VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Dependencies
include(FetchContent)

# nlohmann/json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# yaml-cpp
FetchContent_Declare(
    yaml_cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG master
)
FetchContent_MakeAvailable(yaml_cpp)

# SQLite3
find_package(SQLite3 REQUIRED)

# Create the Clover library
add_library(clover STATIC
    engine/app.cpp
    engine/config.cpp
    engine/database.cpp
    engine/middleware.cpp
    engine/request.cpp
    engine/response.cpp
    engine/router.cpp
    engine/routes.cpp
    engine/server.cpp
)

# Include directories
target_include_directories(clover 
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/engine
)

# Link libraries
target_link_libraries(clover
    PUBLIC
        nlohmann_json::nlohmann_json
        yaml-cpp
        ${SQLite3_LIBRARIES}
)

# Set properties
set_target_properties(clover PROPERTIES
    OUTPUT_NAME clover
)

# Create a simple executable for testing (optional)
add_executable(clover_server engine/main.cpp)
target_link_libraries(clover_server clover)
