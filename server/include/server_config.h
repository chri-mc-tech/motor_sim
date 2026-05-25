#pragma once
#include <iosfwd>
#include "shared_global.h"

namespace config {
  inline int port = DEFAULT_PORT;
  inline int host = 0;
  inline int max_players = 5;
  inline bool debug = false;

  bool check();
  void create_config_file();
  bool load_config();
  void add_config_comment(std::ofstream &file_out);
} // namespace config
