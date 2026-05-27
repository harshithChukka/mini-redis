#pragma once

#include "miniredis/command.hpp"
#include "miniredis/database.hpp"
#include "miniredis/presistence_manager.hpp"

class CommandExecutor {
public:
  CommandExecutor(Database& db, PersistenceManager& pm) : db_(db), pm_(pm) {}
  std::string execute(const Command& cmd, bool skipPersist = false);

private:
  Database& db_;
  PersistenceManager& pm_;
};
