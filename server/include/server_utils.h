#pragma once
#include <string>
#include "server_global.h"

using std::string;

string get_name_from_peer();
Player *get_player_from_name(const string &name);
string enet_ip_to_string(enet_uint32 ip);
