#pragma once
#include <functional>
#include <iostream>
#include <string>

#include "server_logger.h"

class Command {
public:
  std::function<void(string)> command;
  // int permission;

  void execute(string args = "") const;
};

namespace commands {
  void register_commands();
  void process_command(const string &command, const string& args = "");

  void cmd_help(string args = "");
  void cmd_list(string args = "");
  void cmd_kick(string args = "");
}

