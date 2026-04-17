#pragma once
#include <iostream>
#include <string>
#include <yaml-cpp/yaml.h>

class Config {
public:
  static Config &instance() {
    static Config config;
    return config;
  }

  bool load(const std::string &filepath = "config.yaml") {
    try {
      config_ = YAML::LoadFile(filepath);
      filepath_ = filepath;
      std::cout << "Loaded configuration from: " << filepath << std::endl;
      return true;
    } catch (const YAML::Exception &e) {
      std::cerr << "Failed to load config: " << e.what() << std::endl;
      return false;
    }
  }

  // Get value by key (supports nested keys with dot notation)
  template <typename T>
  T get(const std::string &key, const T &default_value = T()) const {
    try {
      YAML::Node node = get_node(key);
      if (node && !node.IsNull()) {
        return node.as<T>();
      }
    } catch (const std::exception &e) {
      std::cerr << "Warning: Could not get config value for '" << key
                << "': " << e.what() << std::endl;
    }
    return default_value;
  }

  // Get string value
  std::string get_string(const std::string &key,
                         const std::string &default_value = "") const {
    return get<std::string>(key, default_value);
  }

  // Get integer value
  int get_int(const std::string &key, int default_value = 0) const {
    return get<int>(key, default_value);
  }

  // Get boolean value
  bool get_bool(const std::string &key, bool default_value = false) const {
    return get<bool>(key, default_value);
  }

  // Check if config has a key
  bool has(const std::string &key) const {
    try {
      YAML::Node node = get_node(key);
      return node && !node.IsNull();
    } catch (...) {
      return false;
    }
  }

  // Get the raw YAML node
  YAML::Node get_node(const std::string &key) const {
    YAML::Node current = config_;
    size_t start = 0;
    size_t end = key.find('.');

    while (end != std::string::npos) {
      std::string part = key.substr(start, end - start);
      if (!current[part]) {
        return YAML::Node();
      }
      current = current[part];
      start = end + 1;
      end = key.find('.', start);
    }

    std::string last_part = key.substr(start);
    return current[last_part];
  }

  // Reload configuration
  bool reload() { return load(filepath_); }

private:
  Config() = default;
  YAML::Node config_;
  std::string filepath_;
};

// Convenience macros for accessing config
#define CFG(key, default) Config::instance().get(key, default)
#define CFG_STR(key, default) Config::instance().get_string(key, default)
#define CFG_INT(key, default) Config::instance().get_int(key, default)
#define CFG_BOOL(key, default) Config::instance().get_bool(key, default)
