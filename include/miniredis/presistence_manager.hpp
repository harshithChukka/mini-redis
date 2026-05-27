#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

#include "miniredis/database.hpp"
#include "miniredis/resp_parser.hpp"

namespace fs = std::filesystem;

constexpr const char* AOF_DIR = "aof";
constexpr const char* AOF_FILE = "aof/appendOnly.aof";

class CommandExecutor;

class PersistenceManager {
public:
  PersistenceManager(Database& db, RespParser& parser)
    : db_(db), parser_(parser) {
    fs::create_directories(AOF_DIR);
    aof_writer_.open(AOF_FILE, std::ios::out | std::ios::app);
    if (!aof_writer_.is_open())
      std::cerr << "persistence_manager: open failed\n";
  }
  void load(CommandExecutor& executor);
  void append(const std::string&);

private:
  Database& db_;
  RespParser& parser_;
  std::ofstream aof_writer_;
};
