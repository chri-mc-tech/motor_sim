#include "server_core.h"

#include <iostream>
#include <thread>

#include "server_network.h"

#include <enet/enet.h>

#include "server_commands.h"
#include "server_global.h"


void server_run() {
  global::running = true;

  auto last = std::chrono::high_resolution_clock::now();

  constexpr int LIMITED_TICKRATE = 50;
  const auto limited_tick_interval = std::chrono::microseconds(1000000 / LIMITED_TICKRATE);
  auto next_limited_tick = std::chrono::steady_clock::now();

  commands::register_commands();

  while (global::running) {
    auto now = std::chrono::high_resolution_clock::now();
    global::delta_time = std::chrono::duration<double>(now - last).count();
    last = now;

    if (std::chrono::steady_clock::now() >= next_limited_tick) {
      enet_loop();
      send_coords();

      next_limited_tick += limited_tick_interval;
    }
  }
}
