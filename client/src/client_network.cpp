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

int create_enet_host() {
  global::enet::enet_client = enet_host_create(nullptr, 1, 3, 0, 0);

  if (global::enet::enet_client == nullptr) {
    return 1;
  }

  return 0;
}

void enet_loop() {
  ENetEvent enet_event;
  while (enet_host_service(global::enet::enet_client, &enet_event, 0) > 0) {
    switch (enet_event.type) {
      case ENET_EVENT_TYPE_CONNECT: enet_event_connected(enet_event); break;
      case ENET_EVENT_TYPE_RECEIVE: enet_event_receive(enet_event); break;
      case ENET_EVENT_TYPE_DISCONNECT: enet_event_disconnected(enet_event); break;
      case ENET_EVENT_TYPE_NONE: break;
    }
    enet_packet_destroy(enet_event.packet);
  }
}

void enet_event_connected(const ENetEvent &enet_event) {
  log_debug("connected");
  global::status_connection = STATUS_CONNECTION_CONNECTED;

  string to_send = shared::network::pkt_type(PKT_FROM_CLIENT_NAME) + config::name;
  ENetPacket *temp_packet = enet_packet_create(to_send.c_str(), to_send.length(), ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(global::enet::connected_server_peer, 0, temp_packet);
}

void enet_event_receive(const ENetEvent &enet_event) {
  string pkt_data_string = shared::utils::packet_to_string(enet_event.packet);

  switch (shared::network::get_pkt_type(pkt_data_string)) {

    case PKT_FROM_SERVER_PLAYER_LIST: {
      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);

      size_t start = 0;

      while (start < pkt_data_string.size()) {
        size_t end = pkt_data_string.find(';', start);

        if (end == std::string::npos)
          end = pkt_data_string.size();
        string player_object = pkt_data_string.substr(start, end - start);


        if (pkt_data_string != config::name) {
          Player temp_player;
          temp_player.name = pkt_data_string;

          global::online_players.emplace(pkt_data_string, std::move(temp_player));
        }
        start = end + 1;
      }
      break;
    }
    case PKT_FROM_SERVER_A_PLAYER_HAS_CONNECTED: {
      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);

      if (pkt_data_string != config::name) {
        Player temp_player;
        temp_player.name = pkt_data_string;

        global::online_players.emplace(pkt_data_string, std::move(temp_player));
      }
      break;
    }

    case PKT_FROM_SERVER_A_PLAYER_HAS_DISCONNECTED: {
      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);
      global::online_players.erase(pkt_data_string);
      break;
    }
    case PKT_FROM_SERVER_COORDS: {
      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);

      size_t start = 0;
      while (start < pkt_data_string.size()) {
        size_t end = pkt_data_string.find(';', start);
        if (end == string::npos) end = pkt_data_string.size();

        string player_object = pkt_data_string.substr(start, end - start);
      }
    }
  }
}



void enet_event_disconnected(const ENetEvent &enet_event) {
  log_debug("disconnected");
  global::status_connection = STATUS_CONNECTION_NOT_CONNECTED;
  global::status_game = STATUS_GAME_NONE;
  global::status_ui = STATUS_UI_DISCONNECTED_FROM_SERVER;
  global::online_players.clear();
}

int connect_to_server(const string& ip, const string& port) {
  ENetAddress server_to_connect;
  enet_address_set_host(&server_to_connect, ip.c_str());
  server_to_connect.port = static_cast<enet_uint16>(std::stoul(port));
  global::enet::connected_server_peer = enet_host_connect(global::enet::enet_client, &server_to_connect, 3, 0);

  global::enet::start_connection_time = std::chrono::steady_clock::now();

  return 0;
}

void send_location() {
    if (global::status_game == STATUS_GAME_PLAYING) {
      float x = global::main_vehicle.current_location.x;
      float z = global::main_vehicle.current_location.z;

      const string to_send =
        std::to_string(x).substr(0, std::to_string(x).find('.') + 3) + " " +
        std::to_string(z).substr(0, std::to_string(z).find('.') + 3);

      shared::network::send_packet(
        global::enet::connected_server_peer,
        PKT_FROM_CLIENT_COORDS,
        to_send, 0);
    }
}