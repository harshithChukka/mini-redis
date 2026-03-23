#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <deque>
#include <mutex>

#include "miniredis/resp_parser.hpp"
#include "miniredis/command_executor.hpp"

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
  Session(boost::asio::io_context& io) : socket_(io) {};
  auto& socket() { return socket_; }

  void start();

private:
  void doRead();
  void doWrite(const std::string&);
  void handleCommand(const size_t&);
  void handleError(const boost::system::error_code&);
  void writeNext();
  bool tryPrepareNextMessage(std::shared_ptr<std::string>& nextMessage);
  void handleWriteComplete(const boost::system::error_code& ec);

  tcp::socket socket_;
  boost::asio::streambuf buffer_;
  RespParser parser_;
  CommandExecutor executor_;
  bool isWriteInProgress = false;
  std::deque<std::string> writeQueue_;
  std::mutex writeMutex_;  
};
