#pragma once
#include <raylib.h>
#include <string>
#include <vector>

using std::string;

class Vehicle {
public:
  // dati statici
  double max_rpm = 0.0;
  double engine_torque = 0.0;

  int total_forward_gears = 0;
  std::vector<double> gear_ratios; // la prima è la retromarcia
  double final_drive = 0.0;

  double total_mass = 0.0;

  double grip = 0.0;
  double downforce_factor = 0.0;

  // dati dinamici
  double current_engine_rpm = 0.0;
  double current_speed = 0.0;
  int current_gear = 0;

  // altro
  Vector3 location = {0, 0, 0};
  Vector3 rotation = {0, 0, 0};

  double wheel_radius = 0.35;

  Mesh mesh;
  Model model;
  Texture texture;

};

namespace vehicles {
  bool validate_transmission(const Vehicle &t_vehicle);
  bool vehicle_file_exist(const string& file_name);
  Vehicle load_vehicle_from_file(const string & file_name);
  void vehicle_debug(const Vehicle &t_vehicle);
}