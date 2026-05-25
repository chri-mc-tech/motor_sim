#pragma once

#include <enet/enet.h>
#include <iostream>

using std::string;

namespace shared::network {
  string from_packet_to_string(const ENetPacket *packet);
  string pkt_type(int int_type);
  bool send_packet(ENetPeer* peer, int pkt_type, string input_string, int flag);
  int get_pkt_type(const string& pkt_string);
}
