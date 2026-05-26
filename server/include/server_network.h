#pragma once
#include <enet/enet.h>
#include <string>
#include "server_global.h"

void enet_loop();
void enet_event_connected();
void enet_event_receive();
void enet_event_disconnected();
bool create_enet_host();
void send_coords();
