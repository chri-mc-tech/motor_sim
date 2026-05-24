#include "client_core.h"

#include <GLFW/glfw3.h>
#include <chrono>
#include <cmath>
#include <raylib.h>
#include <raymath.h>

#include "client_ffb.h"
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

  /*
  main_vehicle.mesh = GenMeshCube(1.5, 0.8, 2.0);
  main_vehicle.model = LoadModelFromMesh(main_vehicle.mesh);
  */

  main_vehicle.model = LoadModel("test_vehicle.glb");

  global::test_track_model = LoadModel("test_circuit.glb");

  auto last = std::chrono::high_resolution_clock::now();


  HideCursor();

  InitAudioDevice();
  Music engine_low = LoadMusicStream("engine_low.wav");
  Music engine_high = LoadMusicStream("engine_high.wav");

  PlayMusicStream(engine_low);
  PlayMusicStream(engine_high);

  ffb_init();


  while (global::running) {
    if (WindowShouldClose()) {
      global::running = false;
    }

    auto now = std::chrono::high_resolution_clock::now();
    global::delta_time = std::chrono::duration<double>(now - last).count();
    last = now;

    update_input();

    UpdateMusicStream(engine_low);
    UpdateMusicStream(engine_high);

    float rpm_ratio = main_vehicle.current_engine_rpm / main_vehicle.max_rpm;

    SetMusicVolume(engine_low, std::max(0.0f, 1.0f - rpm_ratio * 1.5f));
    SetMusicVolume(engine_high, std::min(1.0f, rpm_ratio * 1.5f));

    SetMusicPitch(engine_low, 0.9f + rpm_ratio * 0.8f);
    SetMusicPitch(engine_high, 0.8f + rpm_ratio * 1.0f);


    update_camera();
    physics::physics_loop();

    float max_lat = main_vehicle.grip * 9.81f * (main_vehicle.lateral_stiffness / 6.0f);
    float target_lat_vel = -main_vehicle.current_steer * abs(main_vehicle.current_forward_velocity) * main_vehicle.steer_sensitivity;

    float base_ffb = -main_vehicle.current_lateral_velocity * 0.8f;
    float understeer = abs(target_lat_vel) - max_lat;

    float ffb_force = base_ffb * std::max(0.3f, 1.0f - (understeer * 0.2f));

    if (understeer > 0.0f && abs(main_vehicle.current_forward_velocity) > 3.0f) {
      float vibration_intensity = std::min(understeer * 2.0f, 1.0f);
      float wave = sin(GetTime() * 180.0f);
      float sharp_wave = (wave > 0.0f) ? 1.0f : -1.0f;

      ffb_force += sharp_wave * 1.0f * vibration_intensity;
    }

    ffb_force = std::clamp(ffb_force, -1.0f, 1.0f);

    ffb_update(ffb_force);
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
  camera.fovy = 100.0f;
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

  if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
    int count;
    const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);

    int down_gear = 5;
    int up_gear = 4;

    string gamepad_name = GetGamepadName(GLFW_JOYSTICK_1);

    if (gamepad_name.find("G29") != std::string::npos) {
      main_vehicle.current_steer = std::clamp(static_cast<double>(axes[0] * 4), -1.0, 1.0);
      main_vehicle.current_throttle = 1 - ((axes[1] + 1) / 2);
      main_vehicle.current_brake = 1 - ((axes[2] + 1) / 2);

    }
    else {
      main_vehicle.current_steer = axes[0];
      main_vehicle.current_brake = (axes[4] + 1) / 2;
      main_vehicle.current_throttle = (axes[5] + 1) / 2;

      down_gear = 4;
      up_gear = 5;
    }




    if (main_vehicle.current_forward_velocity < 0.0) {
      main_vehicle.current_steer = -main_vehicle.current_steer;
    }

    const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);

    static bool paddle_right_pressed = false;
    static bool paddle_left_pressed = false;

    if (buttons[up_gear] == GLFW_PRESS && !paddle_right_pressed) {
      if (main_vehicle.current_gear < main_vehicle.total_forward_gears) {
        main_vehicle.current_gear += 1;
      }
    }
    paddle_right_pressed = (buttons[up_gear] == GLFW_PRESS);

    if (buttons[down_gear] == GLFW_PRESS && !paddle_left_pressed) {
      if (main_vehicle.current_gear > -1) {
        main_vehicle.current_gear -= 1;
      }
    }
    paddle_left_pressed = (buttons[down_gear] == GLFW_PRESS);
  }
}

void update_camera() {
  using global::main_vehicle;

  Vector3 localOffset = { 0.0f, 1.2f, 1.0f };
  Vector3 localTarget = { 0.0f, 1.1f, 2.0f };
  Matrix mat = MatrixRotateY(main_vehicle.current_rotation.y);

  graphics::camera.position = Vector3Add(main_vehicle.current_location, Vector3Transform(localOffset, mat));
  graphics::camera.target = Vector3Add(main_vehicle.current_location, Vector3Transform(localTarget, mat));

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
      (main_vehicle.current_rotation.y * RAD2DEG) + 180,
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
