#include "client_config.h"

#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

#include "client_global.h"
#include "client_logger.h"

namespace config {
  bool check() {
    using namespace YAML;

    try {
      Node config = LoadFile("config.yaml");
    } catch (const BadFile&) {
      return false;
    }
    return true;
  }

  void create_config_file() {
    using namespace YAML;
    using std::ofstream;

    Node config;
    config["name"] = "";
    config["log_debug"] = false;
    config["debug_console"] = false;


    ofstream file_out("config.yaml");

    add_config_comment(file_out);

    file_out << config;
    file_out.close();
    log_info("config file created");

  }

  bool load_config() {
    using namespace YAML;

    Node config = LoadFile("config.yaml");
    name = config["name"].as<string>();
    log_debug = config["log_debug"].as<bool>();
    debug_console = config["debug_console"].as<bool>();

    return true;
  }

  void save_new_nickname(const std::string& t_string) {
    using namespace YAML;
    using std::ofstream;

    Node config = LoadFile("config.yaml");
    config["name"] = t_string;
    ofstream file_out("config.yaml");

    add_config_comment(file_out);

    file_out << config;
    file_out.close();
    name = t_string;
  }

  void add_config_comment(std::ofstream &file_out) {
    using std::ofstream;
    file_out << "# Client configuration file\n";
    file_out << "# idk \n";

  }
}