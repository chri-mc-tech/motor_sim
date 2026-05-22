#pragma once
#include <string>
#include <vector>

using std::string;

class Vehicle {
public:

  // dati statici
  double max_rpm = 0.0;
  double engine_torque = 0.0;

  int total_gears = 0; // contando una sola retromarcia e non contando folle
  std::vector<double> gear_ratios; // la prima è la retromarcia
  double final_drive = 0.0;

  double mass = 0.0;

  double grip = 0.0;
  double downforce_factor = 0.0;

  // dati dinamici
  double engine_rpm = 0.0;
  double speed = 0.0;
  int gear = 0;
};

namespace vehicles {
  bool vehicle_file_exist(const string& file_name);
  Vehicle load_vehicle_from_file(const string & file_name);
}