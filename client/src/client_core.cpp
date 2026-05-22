#include "client_core.h"

#include <chrono>
#include <math.h>
#include <raylib.h>
#include "client_global.h"
#include "client_logger.h"
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

  float frame_speed = 8 * global::delta_time;
  float turn_speed = 2.5 * global::delta_time;

  float dir_x = sinf(main_vehicle.rotation.y);
  float dir_z = cosf(main_vehicle.rotation.y);

  if (IsKeyDown(KEY_W)) {
    main_vehicle.location.x += dir_x * frame_speed;
    main_vehicle.location.z += dir_z * frame_speed;
  }

  if (IsKeyDown(KEY_S)) {
    main_vehicle.location.x -= dir_x * frame_speed;
    main_vehicle.location.z -= dir_z * frame_speed;
  }
  if (IsKeyDown(KEY_D)) main_vehicle.rotation.y -= turn_speed;
  if (IsKeyDown(KEY_A)) main_vehicle.rotation.y += turn_speed;

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

void physics_loop() {

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
  EndDrawing();
}
