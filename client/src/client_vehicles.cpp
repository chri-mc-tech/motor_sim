#include "client_vehicles.h"
#include <yaml-cpp/yaml.h>

#include "client_logger.h"

namespace vehicles {
  bool validate_transmission(const Vehicle &t_vehicle) {
    // numero di rapporti è uguale al numero delle marce (+1 per la retromarcia)
    if (t_vehicle.gear_ratios.size() == t_vehicle.total_forward_gears + 1) {
      // il primo rapporto è negativo (retromarcia)
      if (t_vehicle.gear_ratios[0] < 0) {
        return true;
      }
    }
    return false;
  }
  bool vehicle_file_exist(const string &file_name) {
    using namespace YAML;
    try {
      Node config = LoadFile("vehicles/" + file_name);
    } catch (BadFile) {
      return false;
    }
    return true;
  }

  Vehicle load_vehicle_from_file(const string &file_name) {
    using namespace YAML;
    Node vehicle_file = LoadFile("vehicles/" + file_name);
    Vehicle loaded_vehicle;

    bool has_errors = false;

    Node engine_values = vehicle_file["engine"];
    Node transmission_values = vehicle_file["transmission"];
    Node vehicle_values = vehicle_file["vehicle"];
    Node physics_values = vehicle_file["physics"];
    Node steering_values = vehicle_file["steering"];

    if (!engine_values.IsDefined())
      log_error(file_name + ": missing node \"engine\"");
    if (!transmission_values.IsDefined())
      log_error(file_name + ": missing node \"transmission\"");
    if (!vehicle_values.IsDefined())
      log_error(file_name + ": missing node \"vehicle\"");
    if (!physics_values.IsDefined())
      log_error(file_name + ": missing node \"physics\"");
    if (!steering_values.IsDefined())
      log_error(file_name + ": missing node \"steering\"");

    try {
      loaded_vehicle.max_rpm = engine_values["max_rpm"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"max_rpm\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"max_rpm\"");
    }

    try {
      loaded_vehicle.idle_rpm = engine_values["idle_rpm"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"idle_rpm\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"idle_rpm\"");
    }

    try {
      loaded_vehicle.max_engine_torque = engine_values["max_torque"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"max_torque\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"max_torque\"");
    }

    try {
      loaded_vehicle.engine_inertia = engine_values["inertia"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"inertia\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"inertia\"");
    }

    try {
      loaded_vehicle.engine_friction_torque = engine_values["friction_torque"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"friction_torque\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"friction_torque\"");
    }

    try {
      loaded_vehicle.total_forward_gears = transmission_values["total_forward_gears"].as<int>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"total_forward_gears\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"total_forward_gears\"");
    }

    try {
      loaded_vehicle.gear_ratios = transmission_values["gear_ratios"].as<std::vector<double>>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"gear_ratios\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"gear_ratios\"");
    }

    try {
      loaded_vehicle.final_drive = transmission_values["final_drive"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"final_drive\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"final_drive\"");
    }

    try {
      loaded_vehicle.total_mass = vehicle_values["mass"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"mass\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"mass\"");
    }

    try {
      loaded_vehicle.wheel_radius = vehicle_values["wheel_radius"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"wheel_radius\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"wheel_radius\"");
    }

    try {
      loaded_vehicle.aerodynamic_drag = physics_values["aerodynamic_drag"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"aerodynamic_drag\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"aerodynamic_drag\"");
    }

    try {
      loaded_vehicle.rolling_resistance = physics_values["rolling_resistance"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"rolling_resistance\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"rolling_resistance\"");
    }

    try {
      loaded_vehicle.braking_power = physics_values["braking_power"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"braking_power\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"braking_power\"");
    }

    try {
      loaded_vehicle.lateral_stiffness = physics_values["lateral_stiffness"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"lateral_stiffness\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"lateral_stiffness\"");
    }

    try {
      loaded_vehicle.grip = physics_values["grip"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"grip\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"grip\"");
    }

    try {
      loaded_vehicle.downforce_factor = physics_values["downforce_factor"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"downforce_factor\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"downforce_factor\"");
    }

    try {
      loaded_vehicle.wheelbase = steering_values["wheelbase"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"wheelbase\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"wheelbase\"");
    }

    try {
      loaded_vehicle.steer_sensitivity = steering_values["sensitivity"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"sensitivity\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"sensitivity\"");
    }

    try {
      loaded_vehicle.steering_responsiveness = steering_values["responsiveness"].as<double>();
    } catch (BadConversion &error) {
      has_errors = true;
      if (error.mark.is_null())
        log_error(file_name + ": missing parameter \"responsiveness\"");
      else
        log_error(file_name + ": invalid vehicle parameter \"responsiveness\"");
    }

    if (has_errors) {
      return {};
    }

    return loaded_vehicle;
  }

  void vehicle_debug(const Vehicle &t_vehicle) {
    log_debug("max rpm: " + std::to_string(t_vehicle.max_rpm));
    log_debug("engine torque: " + std::to_string(t_vehicle.max_engine_torque));

    log_debug("total forward gears: " + std::to_string(t_vehicle.total_forward_gears));
    log_debug("gear_ratios: ");
    for (const auto t_ratio: t_vehicle.gear_ratios) {
      log_debug(std::to_string(t_ratio));
    }
    log_debug("final drive: " + std::to_string(t_vehicle.final_drive));

    log_debug("vehicle mass: " + std::to_string(t_vehicle.total_mass));

    log_debug("grip: " + std::to_string(t_vehicle.grip));
    log_debug("downforce factor: " + std::to_string(t_vehicle.downforce_factor));
  }


} // namespace vehicles
