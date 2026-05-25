#pragma once
#include <enet/enet.h>
#include <string>
#include <unordered_map>
#include <fstream>
#include "shared_global.h"

class Player {
public:
  std::string name;
  float pos_x = 0;
  float pos_Y = 0;
  float rotation_y = 0;
  ENetPeer* peer;
};


namespace global {
  inline bool running;
  inline std::ofstream log_file;
  inline std::unordered_map<std::string, Player> online_players;
  inline std::unordered_map<ENetPeer *, std::string> peer_to_uuid;
  inline double delta_time;

}

namespace global::enet {
  inline ENetHost* enet_server = nullptr;
  inline ENetAddress address;
}

namespace global::config {
  inline bool debug;
  inline int port = DEFAULT_PORT;
  inline int host = 0;
  inline int max_players = 5;
}