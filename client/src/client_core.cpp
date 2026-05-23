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
  vehicles::validate_transmission(main_vehicle);

  main_vehicle.mesh = GenMeshCube(1.6, 1.0, 2.5);
  main_vehicle.model = LoadModelFromMesh(main_vehicle.mesh);



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

  main_vehicle.throttle = 0.0;
  main_vehicle.steer = 0.0;
  main_vehicle.brake = 0.0;

  if (IsKeyDown(KEY_W)) main_vehicle.throttle = 1.0;
  if (IsKeyDown(KEY_S)) main_vehicle.brake = 1.0;

  if (IsKeyDown(KEY_A)) main_vehicle.steer = 1.0;
  if (IsKeyDown(KEY_D)) main_vehicle.steer = -1.0;

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

  main_vehicle.forward = {
    sinf(main_vehicle.rotation.y),
    0,
    cosf(main_vehicle.rotation.y)
  };

  if (main_vehicle.current_gear == 0)
    main_vehicle.current_gear_ratio = 0;
  else if (main_vehicle.current_gear == -1)
    main_vehicle.current_gear_ratio = main_vehicle.gear_ratios[0];
  else {
    main_vehicle.current_gear_ratio = main_vehicle.gear_ratios[main_vehicle.current_gear];
  }

  main_vehicle.wheels_torque = main_vehicle.engine_torque * main_vehicle.current_gear_ratio * main_vehicle.final_drive;
  main_vehicle.wheels_force = main_vehicle.wheels_torque / main_vehicle.wheel_radius;

  double air_density = 1.225;
  double drag_coefficient = 0.3; // quanto è aereodinamica, minore, piu aereodinamica
  double front_area = 1.9; //quanto è grande davanti, minore, meno resistenza

  double drag_force = 0.5 * air_density * drag_coefficient * front_area * main_vehicle.current_forward_velocity * main_vehicle.current_forward_velocity;


  double coefficiente_attrito_gomme = 0.009; // coefficiente di attrito tra gomme e asfalto, minore, meglio
  double rolling_resistence = coefficiente_attrito_gomme * main_vehicle.total_mass * physics::GRAVITY;


  double total_force = main_vehicle.wheels_force - drag_force - rolling_resistence - main_vehicle.brake;


  main_vehicle.current_forward_acceleration = total_force / main_vehicle.total_mass;

  // main_vehicle.current_forward_acceleration *= main_vehicle.throttle;

  main_vehicle.current_acceleration.x = main_vehicle.forward.x * main_vehicle.current_forward_acceleration;
  main_vehicle.current_acceleration.y = 0;
  main_vehicle.current_acceleration.z = main_vehicle.forward.z * main_vehicle.current_forward_acceleration;

  main_vehicle.current_velocity.x += main_vehicle.current_acceleration.x * global::delta_time;
  main_vehicle.current_velocity.y += main_vehicle.current_acceleration.y * global::delta_time;
  main_vehicle.current_velocity.z += main_vehicle.current_acceleration.z * global::delta_time;

  main_vehicle.current_forward_velocity =
    main_vehicle.current_velocity.x * main_vehicle.forward.x +
    main_vehicle.current_velocity.y * main_vehicle.forward.y +
    main_vehicle.current_velocity.z * main_vehicle.forward.z;


  main_vehicle.location.x += main_vehicle.current_velocity.x * global::delta_time;
  main_vehicle.location.y += main_vehicle.current_velocity.y * global::delta_time;
  main_vehicle.location.z += main_vehicle.current_velocity.z * global::delta_time;

}

void update_camera() {
  using global::main_vehicle;

  Vector3 offset = {
    0.0f,
    1.2f,
    0.0f
  };

  offset.x -= sinf(main_vehicle.rotation.y) * 0.3f;
  offset.z -= cosf(main_vehicle.rotation.y) * 0.3f;

  graphics::camera.position.x = main_vehicle.location.x + offset.x;
  graphics::camera.position.y = main_vehicle.location.y + offset.y;
  graphics::camera.position.z = main_vehicle.location.z + offset.z;

  graphics::camera.target.x = graphics::camera.position.x + sinf(main_vehicle.rotation.y);
  graphics::camera.target.y = graphics::camera.position.y;
  graphics::camera.target.z = graphics::camera.position.z + cosf(main_vehicle.rotation.y);


}


void render_loop() {
  using global::main_vehicle;
  BeginDrawing();
  ClearBackground({100, 150, 200});
  BeginMode3D(graphics::camera);

  DrawCube({0, -1, 0}, 10, 0.2, 10, {110, 50, 160, 255});
  // DrawCube(main_vehicle.location, 1.8, 1.2, 4.5, {20, 20, 100, 255});
  DrawModelEx(
      main_vehicle.model,
      main_vehicle.location,
      { 0.0f, 1.0f, 0.0f },
      main_vehicle.rotation.y * RAD2DEG,
      { 1.0f, 1.0f, 1.0f },
      WHITE
  );

  EndMode3D();
  string debug_text = "\n gear: " + std::to_string(main_vehicle.current_gear) +
                      "\n gear ratio: " + std::to_string(main_vehicle.current_gear_ratio) +
                      "\n engine torque: " + std::to_string(main_vehicle.engine_torque) +
                      "\n wheel torque: " + std::to_string(main_vehicle.wheels_torque) +
                      "\n throttle: " + std::to_string(main_vehicle.throttle) +
                      "\n brake: " + std::to_string(main_vehicle.brake) +
                      "\n acceleration: " + std::to_string(main_vehicle.current_forward_acceleration) +
                      "\n speed: " + std::to_string(main_vehicle.current_forward_velocity) +
                      "\n steer: " + std::to_string(main_vehicle.steer);


  DrawText(debug_text.c_str(), 0, 0, 20, WHITE);
  EndDrawing();
}
