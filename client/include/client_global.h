#pragma once
#include <raylib.h>

namespace graphics {
    inline Camera3D camera;
    inline int window_width = 1280;
    inline int window_height = 720;
}

namespace global {
  inline bool running;
}

#include <vector>

class Vehicle {
public:

  //dati statici
  double max_rpm = 0.0;
  double engine_torque = 0.0;

  std::vector<double> gear_ratios;
  double final_drive = 0.0;

  double mass = 0.0;

  double grip = 0.0;
  double downforce_factor = 0.0;

  //dati dinamici

  double engine_rpm = 0.0;
  double speed = 0.0;
  int gear = 0;
};