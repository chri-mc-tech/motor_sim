#include "server_network.h"

#include "server_core.h"

#include <enet/enet.h>

#include "server_global.h"
#include "server_logger.h"
#include "server_utils.h"
#include "shared_network.h"


int enet_loop() {
  ENetEvent enet_event;

  while (enet_host_service(global::enet::enet_server, &global::enet::enet_event, 0) > 0) {
    switch (enet_event.type) {
      case ENET_EVENT_TYPE_CONNECT: enet_event_connected(); break;
      case ENET_EVENT_TYPE_RECEIVE: enet_event_receive(); break;
      case ENET_EVENT_TYPE_DISCONNECT: enet_event_disconnected(); break;
      case ENET_EVENT_TYPE_NONE: break;
    }
  }
  return 0;
}

void enet_event_connected() {
  log_info("player connected: " + enet_ip_to_string(global::enet::enet_event.peer->address.host));

}

void enet_event_receive() {
  log_info("packet received: ");

}

void enet_event_disconnected() {
  log_info("player disconnected: " + enet_ip_to_string(global::enet::enet_event.peer->address.host));

}

int create_enet_host() {
  global::enet::address.port = global::config::port;
  global::enet::address.host = global::config::host;
  global::enet::enet_server = enet_host_create(&global::enet::address, global::config::max_players, 3, 0, 0);

  if (global::enet::enet_server == nullptr) {
    return 1;
  }

  return 0;
}