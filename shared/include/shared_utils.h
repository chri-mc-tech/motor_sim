#pragma once

#include <enet/enet.h>
#include <iostream>

using std::string;

namespace shared::utils {
  string get_current_time();
  string packet_to_string(const ENetPacket *packet);
}
