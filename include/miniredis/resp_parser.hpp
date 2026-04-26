#pragma once

#include <string>
#include <optional>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/buffers_iterator.hpp>

#include "miniredis/command.hpp"

constexpr char DELIM = ' ';
constexpr char RESP_ARR = '*';

class RespParser {
public:
  std::optional<Command> parse(boost::asio::streambuf&);
  Command parseInline(const std::string&);
};
