#include "client_core.h"

#include <chrono>
#include <cmath>
#include <raylib.h>
#include <GLFW/glfw3.h>
#include "client_global.h"
#include "client_logger.h"
#include "client_physics.h"
#include "client_vehicles.h"

void client_run() {
  using global::main_vehicle;

  global::running = true;


  if (vehicles::vehicle_file_exist("test2.yaml")) {
    main_vehicle = vehicles::load_vehicle_from_file("test2.yaml");
  }
  else {log_error("file not found");}

  vehicles::vehicle_debug(main_vehicle);
  if (!vehicles::validate_transmission(main_vehicle)) {
    log_error("vehicle transmission error");
  }

  main_vehicle.mesh = GenMeshCube(1.5, 0.8, 2.0);
  main_vehicle.model = LoadModelFromMesh(main_vehicle.mesh);


  global::test_track_model = LoadModel("test_circuit.glb");

  auto last = std::chrono::high_resolution_clock::now();




  InitAudioDevice();
  Music engine_idle = LoadMusicStream("engine_1600_idle.wav");
  Music engine_mid  = LoadMusicStream("engine_3400.wav");
  Music engine_high = LoadMusicStream("engine_7200.wav");

  PlayMusicStream(engine_idle);
  PlayMusicStream(engine_mid);
  PlayMusicStream(engine_high);



  log_debug(GetGamepadName(0));


  while (global::running) {
    if (WindowShouldClose()) {
      global::running = false;
    }

    auto now = std::chrono::high_resolution_clock::now();
    global::delta_time = std::chrono::duration<double>(now - last).count();
    last = now;

    update_input();

    UpdateMusicStream(engine_idle);
    UpdateMusicStream(engine_mid);
    UpdateMusicStream(engine_high);

    float rpm_ratio = main_vehicle.current_engine_rpm / main_vehicle.max_rpm;

    float vol_idle = std::max(0.0f, 1.0f - rpm_ratio * 3.0f);
    float vol_mid  = std::max(0.0f, 1.0f - abs(rpm_ratio - 0.5f) * 4.0f);
    float vol_high = std::max(0.0f, (rpm_ratio - 0.33f) * 3.0f);

    SetMusicVolume(engine_idle, vol_idle);
    SetMusicVolume(engine_mid,  vol_mid);
    SetMusicVolume(engine_high, vol_high);

    float pitch_idle = 0.8f + rpm_ratio * 0.4f;
    float pitch_mid  = 0.7f + rpm_ratio * 0.6f;
    float pitch_high = 0.6f + rpm_ratio * 0.8f;

    SetMusicPitch(engine_idle, pitch_idle);
    SetMusicPitch(engine_mid,  pitch_mid);
    SetMusicPitch(engine_high, pitch_high);

    update_camera();
    physics::physics_loop();
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
  main_vehicle.current_brake = 0.0;

  if (IsKeyDown(KEY_W)) main_vehicle.current_throttle = 1.0;
  if (IsKeyDown(KEY_S)) main_vehicle.current_brake = 1.0;

  double steer_speed = 6.0;
  double steer_return = 0.0002;

  if (IsKeyDown(KEY_A)) main_vehicle.current_steer -= steer_speed * global::delta_time;
  else if (IsKeyDown(KEY_D)) main_vehicle.current_steer += steer_speed * global::delta_time;
  else main_vehicle.current_steer *= pow(steer_return, global::delta_time);

  main_vehicle.current_steer = std::clamp(main_vehicle.current_steer, -1.0, 1.0);

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

  if (IsGamepadAvailable(0)) {
    main_vehicle.current_steer = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X); // sterzo, -1 sinistra, 1 destra
    main_vehicle.current_brake = (GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_TRIGGER) + 1) / 2; // freno, -1 mollato, 1 schiacciato
    main_vehicle.current_throttle = (GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_TRIGGER) + 1) / 2; // acceleratore, -1 mollato, 1 schiacciato

    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) {
      if (main_vehicle.current_gear > -1) {
        main_vehicle.current_gear -= 1;
      }
    }
    if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {
      if (main_vehicle.current_gear < main_vehicle.total_forward_gears) {
        main_vehicle.current_gear += 1;
      }
    }

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
                      "\n lateral vel: " + std::to_string(main_vehicle.current_lateral_velocity) +
                      "\n steer: " + std::to_string(main_vehicle.current_steer);


  DrawText(debug_text.c_str(), 0, 0, 20, WHITE);

  int fps = GetFPS();
  DrawText(std::to_string(fps).c_str(), graphics::window_width - 50, 0, 20, WHITE);
  EndDrawing();
}
