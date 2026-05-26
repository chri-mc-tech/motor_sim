#include "server_network.h"

#include <sstream>

#include "server_core.h"

#include <enet/enet.h>

#include "server_config.h"
#include "server_global.h"
#include "server_logger.h"
#include "server_utils.h"
#include "shared_network.h"
#include "shared_utils.h"


void enet_loop() {
  while (enet_host_service(enet::enet_server, &enet::enet_event, 0) > 0) {
    switch (enet::enet_event.type) {
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

void enet_event_receive() {
  // log_info("packet received: ");

  string pkt_data_string = shared::utils::packet_to_string(enet::enet_event.packet);

  switch (shared::network::get_pkt_type(pkt_data_string)) {
    case PKT_FROM_CLIENT_NAME: {
      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);

      log_debug(pkt_data_string);

      if (shared::utils::is_valid_nickname(pkt_data_string)) {
        log_debug("name valid");
      }
      else {log_debug("name NOT valid"); return;}

      if (global::online_players.contains(pkt_data_string)) {
        log_warn("player with name " + pkt_data_string + " already online");
      }
      else {
        Player temp_player;
        temp_player.name = pkt_data_string;
        temp_player.peer = enet::enet_event.peer;

        global::online_players.emplace(pkt_data_string, std::move(temp_player));

      }
      break;
    }
    case PKT_FROM_CLIENT_COORDS: {
      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);

      Player* temp_player = get_player_from_name(get_name_from_peer());

      std::stringstream ss(pkt_data_string);
      string loc_x, loc_z, rot_y;

      if (ss >> loc_x >> loc_z >> rot_y) {
        temp_player->pos_x = stof(loc_x);
        temp_player->pos_z = stof(loc_z);
        temp_player->rot_y = stof(rot_y);
      }

      break;
    }
  }
}

void enet_event_disconnected() {
  log_info("player disconnected: " + enet_ip_to_string(enet::enet_event.peer->address.host));
  global::online_players.erase(get_name_from_peer());

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

void send_coords() {
  using std::to_string;

  string packet_string;
  bool first = true;

  for (const auto& pair : global::online_players) {
    if (!first) {
      packet_string += ";";
    }
    string name = pair.first;
    string str_x = to_string(pair.second.pos_x);
    string str_z = to_string(pair.second.pos_z);
    string str_rot = to_string(pair.second.rot_y);

    packet_string += name + " " +
                     str_x.substr(0, str_x.find('.') + 3) + " " +
                     str_z.substr(0, str_z.find('.') + 3) + " " +
                     str_rot.substr(0, str_rot.find('.') + 3);

    first = false;
  }

  for (const auto& pair : global::online_players) {
    shared::network::send_packet(pair.second.peer, PKT_FROM_SERVER_COORDS, packet_string, 0);
  }
}