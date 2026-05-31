#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <boost/asio/buffers_iterator.hpp>

#include "miniredis/resp_parser.hpp"

namespace {

inline void toLower(std::string& s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });

}

inline std::vector<std::string> splitByDelim(const std::string& s, char delim) {
  std::stringstream ss(s);
  std::vector<std::string> tokens;
  std::string token;
  while (std::getline(ss, token, delim))
    tokens.push_back(token);
  return tokens;
}

}

std::string RespParser::serializeRESP(const Command& cmd) {
  std::ostringstream oss;
  oss << "*" << (cmd.args.size() + 1) << "\r\n";
  oss << "$" << cmd.name.size() << "\r\n";
  oss << cmd.name << "\r\n";

  for (const auto& arg : cmd.args) {
    oss << "$" << arg.size() << "\r\n";
    oss << arg << "\r\n";
  }
  return oss.str();
}

std::optional<Command> RespParser::parse(boost::asio::streambuf& buffer) {
  auto data  = buffer.data();
  auto begin = boost::asio::buffers_begin(data);
  auto end   = boost::asio::buffers_end(data);

  if (begin == end)
    return std::nullopt;

  if (*begin == RESP_ARR)
    return parseRESPArray(begin, end, buffer);

  return parseInlineFromBuffer(begin, end, buffer);
}

std::optional<Command> RespParser::parseRESPArray(
    StreamBufIter& begin,
    StreamBufIter  end,
    boost::asio::streambuf& buffer) {
  auto cursor = begin;

  auto it = std::search(begin, end, CRLF.begin(), CRLF.end());
  if (it == end)
    return std::nullopt;

  int argc = 0;
  try {
    auto numStart = std::next(begin);
    argc = std::stoi(std::string(numStart, it));
  } catch (...) {
    return std::nullopt;
  }
  if (argc <= 0)
    return std::nullopt;

  std::advance(begin, std::distance(begin, it) + CRLF.length());

  Command cmd;
  for (int i = 0; i < argc; ++i) {
    std::string arg;
    if (!parseBulkString(begin, end, arg))
      return std::nullopt;

    if (i == 0) {
      cmd.name = arg;
      toLower(cmd.name);
    } else {
      cmd.args.push_back(arg);
    }
  }

  buffer.consume(std::distance(cursor, begin));
  return cmd;
}

bool RespParser::parseBulkString(
    StreamBufIter& begin,
    StreamBufIter  end,
    std::string&   out) {

  if (begin == end || *begin != '$')
    return false;

  auto it = std::search(begin, end, CRLF.begin(), CRLF.end());
  if (it == end || std::distance(begin, it) < 2)
    return false;

  int len = 0;
  try {
    len = std::stoi(std::string(std::next(begin), it));
  } catch (...) {
    return false;
  }
  if (len < 0)
    return false;

  std::advance(begin, std::distance(begin, it) + CRLF.length());

  if (std::distance(begin, end) < len + 2)
    return false;

  auto dataEnd = begin;
  std::advance(dataEnd, len);
  out.assign(begin, dataEnd);

  auto cr = dataEnd;
  auto lf = std::next(cr);
  if (cr == end || lf == end || *cr != '\r' || *lf != '\n')
    return false;

  std::advance(begin, len + 2);
  return true;
}

std::optional<Command> RespParser::parseInlineFromBuffer(
    StreamBufIter& begin,
    StreamBufIter  end,
    boost::asio::streambuf& buffer) {
  auto it = std::search(begin, end, CRLF.begin(), CRLF.end());
  size_t delimLen = CRLF.length();
  if (it == end) {
    it = std::find(begin, end, '\n');
    delimLen = 1;
  }
  if (it == end)
    return std::nullopt;

  std::string line(begin, it);
  if (!line.empty() && line.back() == '\r')
    line.pop_back();

  buffer.consume(std::distance(begin, it) + delimLen);
  return parseInline(line);
}

Command RespParser::parseInline(const std::string& line) {
  auto tokens = splitByDelim(line, DELIM);
  Command cmd;

  if (tokens.empty()) {
    std::cerr << "[resp_parser] Empty inline command.\n";
    cmd.name = "UNKNOWN";
    return cmd;
  }

  cmd.name = tokens[0];
  toLower(cmd.name);

  if (tokens.size() > 1)
    cmd.args.assign(tokens.begin() + 1, tokens.end());

  return cmd;
}
