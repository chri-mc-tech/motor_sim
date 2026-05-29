#include "client_config.h"
#include "client_core.h"
#include "client_ffb.h"
#include "client_global.h"
#include "client_logger.h"
#include "client_network.h"

int main() {
  if (!config::check()) {
    log_info("no config file found");
    config::create_config_file();
    if (!config::check()) {
      return 4;
    }
  }

  if (!config::load_config()) {
    return 5;
  }

  if (!create_enet_host()) {
    return 6;
  }



  if (!config::name.empty()) {
    global::status_ui = STATUS_UI_MAIN_MENU;
  }


  start_graphics();
  create_log_file();
  client_run();
  // end
  ffb_close();
}
