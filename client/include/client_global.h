#pragma once
#include <chrono>
#include <enet/enet.h>
#include <fstream>
#include <raylib.h>
#include <unordered_map>

#include "client_vehicles.h"

class Player {
public:
  string name;
  float pos_x = 0;
  float pos_z = 0;
  float rot_y = 0;
};

enum status_connection {
  STATUS_CONNECTION_NOT_CONNECTED,
  STATUS_CONNECTION_CONNECTED,
};

enum status_ui {
  STATUS_UI_INPUT_PLAYER_NAME,
  STATUS_UI_MAIN_MENU,
  STATUS_UI_SINGLEPLAYER,
  STATUS_UI_MULTIPLAYER,
  STATUS_UI_DIRECT_CONNECT,
  STATUS_UI_WAITING_USER_INPUT_IP,
  STATUS_UI_CONNECTING,
  STATUS_UI_IN_GAME,
  STATUS_UI_DISCONNECTED_FROM_SERVER,
  STATUS_UI_VOID,
  STATUS_UI_PAUSE,
  STATUS_UI_SETTINGS,
};

enum status_game {
  STATUS_GAME_NONE,
  STATUS_GAME_PLAYING,
};

enum debug_menu { DEBUG_MENU_CLOSED, DEBUG_MENU_DEFAULT, DEBUG_MENU_ADVANCED };

namespace graphics {
  inline Camera3D camera;
  inline int window_width = 1280;
  inline int window_height = 720;
  inline Font font;
} // namespace graphics

namespace global {
  inline bool running;
  inline std::ofstream log_file;
  inline Vehicle main_vehicle;
  inline double delta_time;
  inline double ground_level = 0.0;
  inline bool using_gamepad;

  inline Model test_track_model;

  inline int status_connection = STATUS_CONNECTION_NOT_CONNECTED;
  inline int status_ui = STATUS_UI_INPUT_PLAYER_NAME;
  inline int status_game = STATUS_GAME_NONE;

  inline string input_string;

  inline std::unordered_map<string, Player> online_players;

} // namespace global


namespace enet {
  inline ENetHost *enet_client = nullptr;
  inline ENetPeer *connected_server_peer;
  inline std::chrono::time_point<std::chrono::steady_clock> start_connection_time;
  inline ENetEvent enet_event;
} // namespace enet
