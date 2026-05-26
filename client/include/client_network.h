#pragma once
#define NOGDI

#include <iostream>
#include <string>

#include "shared_global.h"

using std::string;

bool create_enet_host();
void enet_loop();
void enet_event_connected();
void enet_event_receive();
void enet_event_disconnected();
bool connect_to_server(const string &ip, const string &port = std::to_string(DEFAULT_PORT));
void send_location();
