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

std::optional<Command> RespParser::parse(boost::asio::streambuf& buffer) {
  auto data = buffer.data();
  auto begin = boost::asio::buffers_begin(data);
  auto end = boost::asio::buffers_end(data);

  if (begin == end)
    return std::nullopt;

  char first = *begin;
  const std::string delim = "\r\n";

  if (first == RESP_ARR) {
    return std::nullopt;
  } else {
    std::cout << "[resp_parser] Processing as an inline command.\n";
    auto it = std::search(begin, end, delim.begin(), delim.end());
    if (it == end)
      return std::nullopt;

    std::string line(begin, it);
    auto bytes = std::distance(begin, it) + delim.length();
    buffer.consume(bytes);
    return parseInline(line);
  }
}

Command RespParser::parseInline(const std::string& msg) {
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
