#include "client_network.h"

#include "client_network.h"

#include "client_config.h"
#include "client_global.h"
#include "client_ui.h"

#include <thread>
#include <unistd.h>

#include "client_logger.h"
#include "shared_global.h"
#include "shared_network.h"
#include "shared_utils.h"


namespace enet {
  ENetEvent enet_event;
}

bool create_enet_host() {
  enet_initialize();

  enet::enet_client = enet_host_create(nullptr, 1, 3, 0, 0);

  if (enet::enet_client == nullptr) {
    return false;
  }

  return true;
}

void enet_loop() {
  while (enet_host_service(enet::enet_client, &enet::enet_event, 0) > 0) {
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
    enet_packet_destroy(enet::enet_event.packet);
  }
}

void enet_event_connected() {
  log_debug("connected");
  global::status_connection = STATUS_CONNECTION_CONNECTED;
  global::status_ui = STATUS_UI_IN_GAME;
  global::status_game = STATUS_GAME_PLAYING ;

  string to_send = shared::network::pkt_type(PKT_FROM_CLIENT_NAME) + config::name;
  ENetPacket *temp_packet = enet_packet_create(to_send.c_str(), to_send.length(), ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(enet::connected_server_peer, 0, temp_packet);
}


void enet_event_receive() {
  string pkt_data_string = shared::utils::packet_to_string(enet::enet_event.packet);

  switch (shared::network::get_pkt_type(pkt_data_string)) {
    case PKT_FROM_SERVER_COORDS: {
      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);

      size_t start = 0;
      while (start < pkt_data_string.size()) {
        size_t end = pkt_data_string.find(';', start);
        if (end == string::npos) {
          end = pkt_data_string.size();
        }
        string player_object = pkt_data_string.substr(start, end - start);

        std::stringstream ss(player_object);
        string name, loc_x, loc_z, rot_y;

        if (!(ss >> name >> loc_x >> loc_z >> rot_y)) {
          start = end + 1;
          continue;
        }

        auto i = global::online_players.find(name);

        if (i == global::online_players.end()) {
          Player temp_player;
          temp_player.name = name;
          temp_player.pos_x = stof(loc_x);
          temp_player.pos_z = stof(loc_z);
          temp_player.rot_y = stof(rot_y);
          global::online_players.emplace(name, std::move(temp_player));
          log_debug("connected: " + name);
        }
        else {
          Player* temp_player = &i->second;
          temp_player->pos_x = stof(loc_x);
          temp_player->pos_z = stof(loc_z);
          temp_player->rot_y = stof(rot_y);
        }
        start = end + 1;
      }
    }
    default:
      break;
  }
}


void enet_event_disconnected() {
  log_debug("disconnected");
  global::status_connection = STATUS_CONNECTION_NOT_CONNECTED;
  global::status_game = STATUS_GAME_NONE;
  global::status_ui = STATUS_UI_DISCONNECTED_FROM_SERVER;
  global::online_players.clear();
}

bool connect_to_server(const string &ip, const string &port) {
  ENetAddress server_to_connect;
  enet_address_set_host(&server_to_connect, ip.c_str());
  server_to_connect.port = static_cast<enet_uint16>(std::stoul(port));
  enet::connected_server_peer = enet_host_connect(enet::enet_client, &server_to_connect, 3, 0);

  enet::start_connection_time = std::chrono::steady_clock::now();

  return true;
}

void send_location() {
  if (global::status_game == STATUS_GAME_PLAYING) {
    float x = global::main_vehicle.current_location.x;
    float z = global::main_vehicle.current_location.z;
    float rot = global::main_vehicle.current_rotation.y;

    using std::to_string;

    const string to_send = to_string(x).substr(0, to_string(x).find('.') + 3) + " " +
                           to_string(z).substr(0, to_string(z).find('.') + 3) + " " +
                           to_string(rot).substr(0, to_string(rot).find('.') + 3);

    shared::network::send_packet(enet::connected_server_peer, PKT_FROM_CLIENT_COORDS, to_send, 0);
  }
}