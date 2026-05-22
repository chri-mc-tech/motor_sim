#include "client_vehicles.h"
#include <yaml-cpp/yaml.h>

#include "client_logger.h"

namespace vehicles {
  bool vehicle_file_exist(const string& file_name) {
    using namespace YAML;
    try {
      Node config = LoadFile("vehicles/" + file_name);
    }
    catch (BadFile) {
      return false;
    }
    return true;
  }

  Vehicle load_vehicle_from_file(const string& file_name) {
    using namespace YAML;
    Node vehicle_file = LoadFile("vehicles/" + file_name);
    Vehicle loaded_vehicle;

    Node engine_values = vehicle_file["engine"];
    Node transmission_values = vehicle_file["transmission"];
    Node vehicle_values = vehicle_file["vehicle"];
    Node physics_values = vehicle_file["physics"];

    loaded_vehicle.engine_rpm = engine_values["max_rpm"].as<double>();
    loaded_vehicle.engine_torque = engine_values["torque"].as<double>();

    loaded_vehicle.total_gears = transmission_values["total_gears"].as<int>();
    loaded_vehicle.gear_ratios = transmission_values["gear_ratios"].as<std::vector<double>>();
    loaded_vehicle.final_drive = transmission_values["final_drive"].as<double>();

    loaded_vehicle.mass = vehicle_values["mass"].as<double>();

    loaded_vehicle.grip = physics_values["grip"].as<double>();
    loaded_vehicle.downforce_factor = physics_values["downforce_factor"].as<double>();


    return loaded_vehicle;
  }


}