#pragma once
#include <fstream>
#include <raylib.h>

#include "client_vehicles.h"

namespace graphics {
    inline Camera3D camera;
    inline int window_width = 1280;
    inline int window_height = 720;
}

namespace global {
  inline bool running;
  inline std::ofstream log_file;
  inline Vehicle main_vehicle;
  inline double delta_time;
}

namespace config {
  inline bool debug_console = true;
  inline bool log_debug = true;
}