#include "client_physics.h"

#include <bits/algorithmfwd.h>
#include <cmath>

#include "client_global.h"

namespace physics {
  void physics_loop() {
    using global::main_vehicle;
    using global::delta_time;

    double peak_rpm = (main_vehicle.max_rpm / 4) * 3;
    main_vehicle.current_engine_torque = main_vehicle.max_engine_torque * (1 - pow(((main_vehicle.current_engine_rpm - peak_rpm) / peak_rpm), 2));


    if (main_vehicle.current_engine_rpm >= main_vehicle.max_rpm) {
      if (fmod(GetTime() * 15.0, 1.0) > 0.5) {
        main_vehicle.current_engine_rpm = main_vehicle.max_rpm * 0.85;
        main_vehicle.current_engine_torque = -main_vehicle.max_engine_torque * 0.9;
      }
    } else {
      main_vehicle.current_engine_rpm = std::clamp(main_vehicle.current_engine_rpm, main_vehicle.idle_rpm, main_vehicle.max_rpm);
    }


    if (main_vehicle.current_gear == 0)
      main_vehicle.current_gear_ratio = 0;
    else if (main_vehicle.current_gear == -1)
      main_vehicle.current_gear_ratio = main_vehicle.gear_ratios[0];
    else {
      main_vehicle.current_gear_ratio = main_vehicle.gear_ratios[main_vehicle.current_gear];
    }

    // calcolo coppia ruote
    main_vehicle.current_wheels_torque = main_vehicle.current_engine_torque * main_vehicle.current_throttle * main_vehicle.current_gear_ratio * main_vehicle.final_drive;
    // calcolo forza che le ruote applicano sul loro bordo
    main_vehicle.current_wheels_force = main_vehicle.current_wheels_torque / main_vehicle.wheel_radius;

    double downforce = main_vehicle.downforce_factor * main_vehicle.current_forward_velocity * abs(main_vehicle.current_forward_velocity);
    double normal_force = main_vehicle.total_mass * physics::GRAVITATIONAL_FORCE + downforce;

    double max_traction_force = main_vehicle.grip * normal_force;
    double excess = abs(main_vehicle.current_wheels_force) - max_traction_force;
    if (excess > 0) {
      int sign = main_vehicle.current_wheels_force > 0 ? 1 : -1;
      main_vehicle.current_wheels_force = max_traction_force * sign;
    }

    double air_resistance = main_vehicle.aerodynamic_drag * main_vehicle.current_forward_velocity * abs(main_vehicle.current_forward_velocity);
    double rolling_resistance = main_vehicle.rolling_resistance * main_vehicle.current_forward_velocity;

    int temp;
    if (main_vehicle.current_forward_velocity == 0) {temp = 0;}
    else if (main_vehicle.current_forward_velocity > 0) {temp = 1;}
    else if (main_vehicle.current_forward_velocity < 0) {temp = -1;}

    double brake_force = main_vehicle.braking_power * main_vehicle.current_brake * temp;

    double engine_braking = 0.0;
    if (main_vehicle.current_throttle == 0 && main_vehicle.current_gear != 0) {
      double braking_torque_at_wheels = main_vehicle.engine_friction_torque * abs(main_vehicle.current_gear_ratio) * main_vehicle.final_drive;

      engine_braking = (braking_torque_at_wheels / main_vehicle.wheel_radius) * temp;
    }

    main_vehicle.current_wheels_force = main_vehicle.current_wheels_force - air_resistance - rolling_resistance - brake_force - engine_braking;

    // calcolo accelerazione
    main_vehicle.current_forward_acceleration = main_vehicle.current_wheels_force / main_vehicle.total_mass;
    // calcolo velocità
    main_vehicle.current_forward_velocity += main_vehicle.current_forward_acceleration * delta_time;

    if (abs(main_vehicle.current_forward_velocity) < 0.1) {
      if (main_vehicle.current_wheels_torque == 0) {
        main_vehicle.current_forward_velocity = 0;
        main_vehicle.current_forward_acceleration = 0;
      }
    }

    if (main_vehicle.current_gear == 0) {
      double engine_net_torque = (main_vehicle.current_engine_torque * main_vehicle.current_throttle) - main_vehicle.engine_friction_torque;

      double angular_acceleration = engine_net_torque / main_vehicle.engine_inertia;

      double rpm_acceleration = angular_acceleration * (60.0 / (2.0 * std::numbers::pi));

      main_vehicle.current_engine_rpm += rpm_acceleration * delta_time;
    }

    else {
      double wheels_angular_velocity = main_vehicle.current_forward_velocity / main_vehicle.wheel_radius;

      if (excess > 0) {
        wheels_angular_velocity += (excess / (main_vehicle.total_mass * 1.5));
      }

      main_vehicle.current_engine_rpm = abs(wheels_angular_velocity) * main_vehicle.current_gear_ratio * main_vehicle.final_drive * (60.0 / (2.0 * std::numbers::pi));
    }

    main_vehicle.current_engine_rpm = std::clamp(main_vehicle.current_engine_rpm, main_vehicle.idle_rpm, main_vehicle.max_rpm);

    double target_lat_vel = -main_vehicle.current_steer * abs(main_vehicle.current_forward_velocity) * main_vehicle.steer_sensitivity;

    main_vehicle.current_lateral_velocity += (target_lat_vel - main_vehicle.current_lateral_velocity) * main_vehicle.steering_responsiveness * delta_time;

    double max_lat_limit = main_vehicle.grip * physics::GRAVITATIONAL_FORCE * (main_vehicle.lateral_stiffness / 6.0);
    main_vehicle.current_lateral_velocity = std::clamp(main_vehicle.current_lateral_velocity, -max_lat_limit, max_lat_limit);

    double yaw_rate = main_vehicle.current_lateral_velocity / main_vehicle.wheelbase;
    main_vehicle.current_rotation.y += yaw_rate * delta_time;

    main_vehicle.current_forward.x = sin(main_vehicle.current_rotation.y);
    main_vehicle.current_forward.z = cos(main_vehicle.current_rotation.y);
    main_vehicle.current_right.x = cos(main_vehicle.current_rotation.y);
    main_vehicle.current_right.z = -sin(main_vehicle.current_rotation.y);

    main_vehicle.current_location.x += (main_vehicle.current_forward.x * main_vehicle.current_forward_velocity + main_vehicle.current_right.x * main_vehicle.current_lateral_velocity) * delta_time;
    main_vehicle.current_location.z += (main_vehicle.current_forward.z * main_vehicle.current_forward_velocity + main_vehicle.current_right.z * main_vehicle.current_lateral_velocity) * delta_time;


    main_vehicle.current_velocity.y = main_vehicle.current_velocity.y - (physics::GRAVITATIONAL_FORCE * delta_time);
    main_vehicle.current_location.y = main_vehicle.current_location.y + (main_vehicle.current_velocity.y * delta_time);

    if (main_vehicle.current_location.y < global::ground_level) {
      main_vehicle.current_location.y = global::ground_level;
      main_vehicle.current_velocity.y = 0;
    }
  }

}