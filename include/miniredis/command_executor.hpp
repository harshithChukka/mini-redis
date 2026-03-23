#pragma once

#include "miniredis/command.hpp"
#include "miniredis/database.hpp"

class CommandExecutor {
public:
  std::string execute(const Command& cmd);

private:
  Database db_;
};
