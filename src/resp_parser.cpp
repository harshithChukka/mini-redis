#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

#include "miniredis/resp_parser.hpp"

inline std::vector<std::string> split(const std::string& s, char delim) {
  std::stringstream ss(s);
  std::vector<std::string> result;
  std::string item;

  while (std::getline(ss, item, delim))
    result.push_back(item);

  return result;
}

Command RespParser::parse(const std::string& msg) {
  std::vector<std::string> tokens = split(msg, DELIM);
  Command cmd = { .name = "UNKNOWN" };

  if (tokens.empty()) {
    std::cerr << "[resp_parser] Empty input message received.\n";
    return cmd;
  }

  cmd.name = tokens[0];
  std::transform(cmd.name.begin(), cmd.name.end(), cmd.name.begin(), [](unsigned char c) { return std::tolower(c); });

  if (tokens.size() > 1) {
    cmd.args.assign(tokens.begin() + 1, tokens.end());
  }

  return cmd;
}
