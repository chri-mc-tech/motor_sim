#pragma once
#include "client_ui.h"

void start_graphics();
void render_loop();
void client_run();
void update_input();
void update_camera();
void render_3D();
void render_menu();
void get_keyboard_input();
bool is_button_clicked(const ui::Button &button);
