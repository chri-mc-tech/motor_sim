#pragma once
#include <string>
#include "server_global.h"

using std::string;

Player *get_player_from_name(const string &name);
string enet_ip_to_string(enet_uint32 ip);
