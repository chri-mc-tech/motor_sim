#include "server_core.h"
#include "server_logger.h"
#include "shared_network.h"

#include <iostream>
#include <thread>

#include "server_config.h"
#include "server_console.h"
#include "server_global.h"
#include "server_network.h"

using namespace std;

int main() {
  enet_initialize();

  if (!config::check()) {
    log_info("Config file not found, creating default...");
    config::create_config_file();
    if (!config::check()) {
      log_error("Error creating config file");
      return 4;
    }
  } else {
    log_info("Config file found, loading...");
  }

  if (!config::load_config()) {
    log_error("Error loading config file");
    return 5;
  }
  log_info("Server configuration loaded successfully");

  if (!create_enet_host()) {
    return 6;
  }


  log_info("info_test");
  log_warn("warn_test");
  log_error("error_test");

  log_debug("Debug mode enabled");

  jthread thread_console(console::console);
  // create_log_file();
  server_run();
}
