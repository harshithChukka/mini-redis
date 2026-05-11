#include <algorithm>
#include <cctype>
#include <iostream>
#include <string_view>

#include "miniredis/command_executor.hpp"

namespace {
constexpr std::string_view UNKNOWN_COMMAND = "Unknown Command";

std::string wrongArgCnt(std::string_view commandName, std::size_t expected, std::size_t got) {
  return "ERR wrong number of arguments for '" + std::string(commandName)
    + "' command (expected " + std::to_string(expected)
    + ", got " + std::to_string(got) + ")";
}

std::string simpleString(const std::string& s) {
    return "+" + s + "\r\n";
}

std::string bulkString(const std::string& s) {
    return "$" + std::to_string(s.size()) + "\r\n" + s + "\r\n";
}

std::string nullBulk() {
    return "$-1\r\n";
}

template <typename T>
std::string integer(T val) {
    return ":" + std::to_string(val) + "\r\n";
}

std::string error(const std::string& msg) {
    return "-ERR " + msg + "\r\n";
}
}

std::string CommandExecutor::execute(const Command& cmd) {
  std::string commandName = cmd.name;
  std::cout << "[command_executor] Executing command: " << cmd.name << "\n";

  if (commandName == "set") {
    if (cmd.args.size() != 2) return wrongArgCnt(commandName, 2, cmd.args.size());
    db_.set(cmd.args[0], cmd.args[1]);
    return simpleString("OK");
  }

  if (commandName == "get") {
    if (cmd.args.size() != 1) return wrongArgCnt(commandName, 1, cmd.args.size());
    auto val = db_.get(cmd.args[0]);
    if (!val)
      return nullBulk();
    return bulkString(*val);
  }

  if (commandName == "del") {
    if (cmd.args.size() != 1) return wrongArgCnt(commandName, 1, cmd.args.size());
    int removed = db_.del(cmd.args[0]);
    return integer<int>(removed);
  }

  if (commandName == "ping") {
    int argc = cmd.args.size();
    if (argc == 1) return bulkString(cmd.args[0]);
    else if (argc == 0) return simpleString("PONG");
    return wrongArgCnt(commandName, 1, argc);
  }

  if (commandName == "incr") {
    if (cmd.args.size() != 1) return wrongArgCnt(commandName, 1, cmd.args.size());
    auto val = db_.incr(cmd.args[0]);
    if (!val)
      return error("value is not an integer");
    return integer<int64_t>(*val);
  }

  std::cerr << "[command_executor] Unknown command received: " << cmd.name << "\n";
  return error(std::string(UNKNOWN_COMMAND));
}
