#include "client_core.h"
#include <raylib.h>
#include "client_global.h"
#include "client_vehicles.h"

void client_run() {
  global::running = true;

  if (vehicles::vehicle_file_exist("test.yaml"))
  vehicles::load_vehicle_from_file("test.yaml");

  while (global::running) {
    if (WindowShouldClose()) {
      global::running = false;
    }

    main_loop();
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

void main_loop() {
  BeginDrawing();
  ClearBackground({100, 150, 200});
  BeginMode3D(graphics::camera);
  EndMode3D();
  EndDrawing();
}
