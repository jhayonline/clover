#include "../include/app.hpp"
#include "../include/config.hpp"
#include "../include/server.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  std::cout << "🍀 Clover Server starting..." << std::endl;

  // Load configuration
  std::string config_file = "config.yaml";
  if (argc > 1) {
    config_file = argv[1];
  }

  Config::instance().load(config_file);

  // Get app environment
  std::string env = CFG_STR("app.env", "development");
  bool debug = CFG_BOOL("app.debug", true);

  if (debug) {
    std::cout << "Running in " << env << " mode" << std::endl;
  }

  // Build router from user's App::routes()
  Router router = App::routes();

  // Create server with config values
  Server server;

  // Override with command line args if provided
  if (argc > 3) {
    Server server(argv[2], std::stoi(argv[3]));
  }

  // Attach router
  server.use(router);

  // Start server
  std::cout << "Clover is ready!" << std::endl;
  server.listen();

  return 0;
}
