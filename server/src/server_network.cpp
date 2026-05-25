#include "server_network.h"

#include "server_core.h"

#include <enet/enet.h>

#include "server_global.h"
#include "server_logger.h"
#include "server_utils.h"
#include "shared_network.h"


int enet_loop() {
  ENetEvent enet_event;

  while (enet_host_service(global::enet::enet_server, &enet_event, 0) > 0) {
    switch (enet_event.type) {
      case ENET_EVENT_TYPE_CONNECT: enet_event_connected(enet_event); break;
      case ENET_EVENT_TYPE_RECEIVE: enet_event_receive(enet_event); break;
      case ENET_EVENT_TYPE_DISCONNECT: enet_event_disconnected(enet_event); break;
      case ENET_EVENT_TYPE_NONE: break;
    }
  }
  return 0;
}

void enet_event_connected(const ENetEvent &enet_event) {
  log_info("player connected: " + enet_ip_to_string(enet_event.peer->address.host));

}
