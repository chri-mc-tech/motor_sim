#include "shared_utils.h"

#include <chrono>
#include <enet/enet.h>
#include <iostream>

using std::string;

namespace shared::utils {
  string get_current_time() {
    time_t t = time(nullptr);
    tm *now = localtime(&t);
    char buf[9];
    strftime(buf, sizeof(buf), "%H:%M:%S", now);
    return string(buf);
  }


  string packet_to_string(const ENetPacket *packet) {
    string string(reinterpret_cast<char*>(packet->data), packet->dataLength);
    return string;
  }

}


