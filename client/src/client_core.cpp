#include "client_core.h"

#include <chrono>
#include <cmath>
#include <raylib.h>
#include "client_global.h"
#include "client_logger.h"
#include "client_physics.h"
#include "client_vehicles.h"

void client_run() {
  using global::main_vehicle;

  global::running = true;

  if (vehicles::vehicle_file_exist("test.yaml")) {
    main_vehicle = vehicles::load_vehicle_from_file("test.yaml");
  }
  else {log_error("file not found");}

  vehicles::vehicle_debug(main_vehicle);
  if (!vehicles::validate_transmission(main_vehicle)) {
    log_error("vehicle transmission error");
  }

  main_vehicle.mesh = GenMeshCube(1.5, 0.8, 2.0);
  main_vehicle.model = LoadModelFromMesh(main_vehicle.mesh);

  Mesh track_test_mesh = GenMeshCube(100, 0.2, 100);
  global::test_track_model = LoadModelFromMesh(track_test_mesh);

  Texture texture = LoadTexture("test_texture.png");

  global::test_track_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

  auto last = std::chrono::high_resolution_clock::now();

  while (global::running) {
    if (WindowShouldClose()) {
      global::running = false;
    }

    auto now = std::chrono::high_resolution_clock::now();
    global::delta_time = std::chrono::duration<double>(now - last).count();
    last = now;

    update_input();
    update_camera();
    physics_loop();
    render_loop();
  }


}

void start_graphics() {
  using namespace graphics;

  InitWindow(window_width, window_height, "Motor Sim");

  // auto logo = LoadImage("logo.png");
  // SetWindowIcon(logo);

  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetWindowMinSize(640, 360);
  SetWindowMaxSize(7680, 4320);

  camera.position = (Vector3) {0.0f, 2, 0.0f};
  camera.target = (Vector3) {0.0f, 2, 1.0f};
  camera.up = (Vector3) {0.0f, 3.0f, 0.0f};
  camera.fovy = 110.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  SetExitKey(KEY_NULL);
}

void update_input() {
  using global::main_vehicle;

  main_vehicle.current_throttle = 0.0;
  main_vehicle.current_steer = 0.0;
  main_vehicle.current_brake = 0.0;

  if (IsKeyDown(KEY_W)) main_vehicle.current_throttle = 1.0;
  if (IsKeyDown(KEY_S)) main_vehicle.current_brake = 1.0;

  if (IsKeyDown(KEY_A)) main_vehicle.current_steer = -1.0;
  if (IsKeyDown(KEY_D)) main_vehicle.current_steer = 1.0;

  if (IsKeyPressed(KEY_Q)) {
    if (main_vehicle.current_gear > -1) {
      main_vehicle.current_gear -= 1;
    }
  }
  if (IsKeyPressed(KEY_E)) {
    if (main_vehicle.current_gear < main_vehicle.total_forward_gears) {
      main_vehicle.current_gear += 1;
    }
  }
}

void physics_loop() {
  using global::main_vehicle;
  using global::delta_time;

  double peak_rpm = (main_vehicle.max_rpm / 4) * 3;
  main_vehicle.current_engine_torque = main_vehicle.max_engine_torque * (1 - pow(((main_vehicle.current_engine_rpm - peak_rpm) / peak_rpm), 2));

  if (main_vehicle.current_engine_rpm >= main_vehicle.max_rpm) {
    main_vehicle.current_engine_torque = 0;
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

  double aerodynamic_constant = 0.42;
  double air_resistance = aerodynamic_constant * main_vehicle.current_forward_velocity * abs(main_vehicle.current_forward_velocity);
  double mechanical_and_asphalt_friction_constant = 12.0;
  double rolling_resistance = mechanical_and_asphalt_friction_constant * main_vehicle.current_forward_velocity;

  double brake_coefficient = 20000.0;
  int temp;
  if (main_vehicle.current_forward_velocity == 0) {temp = 0;}
  else if (main_vehicle.current_forward_velocity > 0) {temp = 1;}
  else if (main_vehicle.current_forward_velocity < 0) {temp = -1;}

  double brake_force = brake_coefficient * main_vehicle.current_brake * temp;

  main_vehicle.current_wheels_force = main_vehicle.current_wheels_force - air_resistance - rolling_resistance - brake_force;

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

  double idle_rpm = 900.0;

  if (main_vehicle.current_gear == 0) {
    if (main_vehicle.current_throttle == 0) {
      main_vehicle.current_engine_rpm -= 2000.0 * delta_time;
    }
    else {
      main_vehicle.current_engine_rpm += 7000.0 * main_vehicle.current_throttle * delta_time;
    }
  }

  else {
    double wheels_angular_velocity = main_vehicle.current_forward_velocity / main_vehicle.wheel_radius;
    main_vehicle.current_engine_rpm = abs(wheels_angular_velocity) * main_vehicle.current_gear_ratio * main_vehicle.final_drive * (60 / (2 * std::numbers::pi));
  }

  main_vehicle.current_engine_rpm = std::clamp(main_vehicle.current_engine_rpm, idle_rpm, main_vehicle.max_rpm);

  double steer_sensitivity = 0.4;

  main_vehicle.current_rotation.y = main_vehicle.current_rotation.y + (main_vehicle.current_steer * main_vehicle.current_forward_velocity * -steer_sensitivity * delta_time);

  main_vehicle.current_forward.x = sin(main_vehicle.current_rotation.y);
  main_vehicle.current_forward.z = cos(main_vehicle.current_rotation.y);

  main_vehicle.current_location.x = main_vehicle.current_location.x + (main_vehicle.current_forward.x * main_vehicle.current_forward_velocity * delta_time);
  main_vehicle.current_location.z = main_vehicle.current_location.z + (main_vehicle.current_forward.z * main_vehicle.current_forward_velocity * delta_time);


  main_vehicle.current_velocity.y = main_vehicle.current_velocity.y - (physics::GRAVITATIONAL_FORCE * delta_time);
  main_vehicle.current_location.y = main_vehicle.current_location.y + (main_vehicle.current_velocity.y * delta_time);

  if (main_vehicle.current_location.y < global::ground_level) {
    main_vehicle.current_location.y = global::ground_level;
    main_vehicle.current_velocity.y = 0;
  }
}

void update_camera() {
  using global::main_vehicle;

  Vector3 offset = {
    0.0f,
    1.2f,
    0.0f
  };

  offset.x -= sinf(main_vehicle.current_rotation.y) * 0.3f;
  offset.z -= cosf(main_vehicle.current_rotation.y) * 0.3f;

  graphics::camera.position.x = main_vehicle.current_location.x + offset.x;
  graphics::camera.position.y = main_vehicle.current_location.y + offset.y;
  graphics::camera.position.z = main_vehicle.current_location.z + offset.z;

  graphics::camera.target.x = graphics::camera.position.x + sinf(main_vehicle.current_rotation.y);
  graphics::camera.target.y = graphics::camera.position.y;
  graphics::camera.target.z = graphics::camera.position.z + cosf(main_vehicle.current_rotation.y);


}


void render_loop() {
  using global::main_vehicle;
  BeginDrawing();
  ClearBackground({100, 150, 200});
  BeginMode3D(graphics::camera);

  DrawModel(global::test_track_model, {0, 0, 0}, 1, WHITE);
  // DrawCube(main_vehicle.location, 1.8, 1.2, 4.5, {20, 20, 100, 255});
  DrawModelEx(
      main_vehicle.model,
      main_vehicle.current_location,
      { 0.0f, 1.0f, 0.0f },
      main_vehicle.current_rotation.y * RAD2DEG,
      { 1.0f, 1.0f, 1.0f },
      WHITE
  );

  EndMode3D();
  string debug_text = "\n gear: " + std::to_string(main_vehicle.current_gear) +
                      "\n gear ratio: " + std::to_string(main_vehicle.current_gear_ratio) +
                      "\n engine max torque: " + std::to_string(main_vehicle.max_engine_torque) +
                      "\n engine torque: " + std::to_string(main_vehicle.current_engine_torque) +
                      "\n wheel torque: " + std::to_string(main_vehicle.current_wheels_torque) +
                      "\n throttle: " + std::to_string(main_vehicle.current_throttle) +
                      "\n brake: " + std::to_string(main_vehicle.current_brake) +
                      "\n acceleration: " + std::to_string(main_vehicle.current_forward_acceleration) +
                      "\n speed (m/s): " + std::to_string(main_vehicle.current_forward_velocity) +
                      "\n speed (km/h): " + std::to_string(main_vehicle.current_forward_velocity * 3.6) +
                      "\n rpm: " + std::to_string(main_vehicle.current_engine_rpm) +
                      "\n steer: " + std::to_string(main_vehicle.current_steer);


  DrawText(debug_text.c_str(), 0, 0, 20, WHITE);
  EndDrawing();
}
