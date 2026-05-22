#include "client_core.h"

#include <chrono>
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

  main_vehicle.mesh = GenMeshCube(1.8, 1.2, 4.5);
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

  float frame_speed = 8 * static_cast<float>(global::delta_time);

  UpdateCameraPro(&graphics::camera, {
      (static_cast<float>(IsKeyDown(KEY_W)) - static_cast<float>(IsKeyDown(KEY_S))) * frame_speed,
      0,0},
      {(static_cast<float>(IsKeyDown(KEY_D)) - static_cast<float>(IsKeyDown(KEY_A))) * static_cast<float>(global::delta_time) * 50, 0, 0}, 0);

  main_vehicle.location = graphics::camera.position;
  main_vehicle.location.y -= 1;



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
    (Vector3){ 0.0f, 1.0f, 0.0f },
    main_vehicle.rotation.y,
    (Vector3){ 1.0f, 1.0f, 1.0f },
    WHITE
);

  EndMode3D();
  EndDrawing();
}
