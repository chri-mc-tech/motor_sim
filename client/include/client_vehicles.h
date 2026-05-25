#pragma once
#include <raylib.h>
#include <string>
#include <vector>

using std::string;

class Vehicle {
public:
  // Dati statici (caricati da file YAML)
  double max_rpm = 0.0;
  double idle_rpm = 0.0;
  double max_engine_torque = 0.0;
  double engine_inertia = 0.0;
  double engine_friction_torque = 0.0;

  int total_forward_gears = 0;
  std::vector<double> gear_ratios; // la prima è la retromarcia
  double final_drive = 0.0;

  double total_mass = 0.0;
  double wheel_radius = 0.0;

  double grip = 0.0;
  double downforce_factor = 0.0;
  double aerodynamic_drag = 0.0;
  double rolling_resistance = 0.0;
  double braking_power = 0.0;
  double lateral_stiffness = 0.0;

  double wheelbase = 0.0;
  double steer_sensitivity = 0.0;
  double steering_responsiveness = 0.0;

  // Dati dinamici (gestiti dalla fisica in tempo reale)
  double current_forward_velocity = 0.0;
  double current_lateral_velocity = 0.0;
  double current_forward_acceleration = 0.0;
  double current_engine_rpm = 0.0;
  int current_gear = 0;
  double current_gear_ratio = 0;
  double current_throttle = 0.0;
  double current_brake = 0.0;
  double current_steer = 0.0;
  double current_wheels_torque = 0.0;
  double current_wheels_force = 0.0;
  double current_engine_torque = 0.0;

  Vector3 current_forward = {0, 0, 1};
  Vector3 current_right = {1, 0, 0};
  Vector3 current_velocity = {0, 0, 0};
  Vector3 current_acceleration = {0, 0, 0};

  Vector3 current_location = {0, 0, 0};
  Vector3 current_rotation = {0, 0, 0};

  // Grafica
  Mesh mesh;
  Model model;
  Texture texture;
};

namespace vehicles {
  bool validate_transmission(const Vehicle &t_vehicle);
  bool vehicle_file_exist(const string &file_name);
  Vehicle load_vehicle_from_file(const string &file_name);
  void vehicle_debug(const Vehicle &t_vehicle);
} // namespace vehicles
