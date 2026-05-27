#pragma once

#include <string>
#include <optional>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/buffers_iterator.hpp>

#include "miniredis/command.hpp"

constexpr char DELIM = ' ';
constexpr char RESP_ARR = '*';
constexpr std::string_view CRLF = "\r\n";

using StreamBufIter = boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type>;

class RespParser {
public:
  std::optional<Command> parse(boost::asio::streambuf&);
  Command parseInline(const std::string&);
  std::string serializeRESP(const Command& cmd);
private:
  std::optional<Command> parseRESPArray(StreamBufIter& begin, StreamBufIter end, boost::asio::streambuf& buffer);
  std::optional<Command> parseInlineFromBuffer(StreamBufIter& begin, StreamBufIter end, boost::asio::streambuf& buffer);
  bool parseBulkString(StreamBufIter& begin, StreamBufIter end, std::string& out);
};
