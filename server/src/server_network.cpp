#include "server_network.h"

#include "server_core.h"

#include <enet/enet.h>

#include "server_config.h"
#include "server_global.h"
#include "server_logger.h"
#include "server_utils.h"
#include "shared_network.h"


void enet_loop() {
  ENetEvent enet_event;

  while (enet_host_service(enet::enet_server, &enet::enet_event, 0) > 0) {
    switch (enet_event.type) {
      case ENET_EVENT_TYPE_CONNECT:
        enet_event_connected();
        break;
      case ENET_EVENT_TYPE_RECEIVE:
        enet_event_receive();
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        enet_event_disconnected();
        break;
      case ENET_EVENT_TYPE_NONE:
        break;
    }
  }
}

void enet_event_connected() {
  log_info("player connected: " + enet_ip_to_string(enet::enet_event.peer->address.host));
}

void enet_event_receive() { log_info("packet received: "); }

void enet_event_disconnected() {
  log_info("player disconnected: " + enet_ip_to_string(enet::enet_event.peer->address.host));
}

bool create_enet_host() {
  enet::address.port = config::port;
  enet::address.host = config::host;
  enet::enet_server = enet_host_create(&enet::address, config::max_players, 3, 0, 0);

  if (enet::enet_server == nullptr) {
    return false;
  }

  return true;
}
