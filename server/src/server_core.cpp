#include "server_core.h"

#include <thread>

#include "server_network.h"

#include <enet/enet.h>
#include "server_global.h"


void server_run() {
  global::running = true;

  auto last = std::chrono::high_resolution_clock::now();


  while (global::running) {
    auto now = std::chrono::high_resolution_clock::now();
    global::delta_time = std::chrono::duration<double>(now - last).count();
    last = now;


  }
}

