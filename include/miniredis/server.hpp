#pragma once

#include <boost/asio.hpp>

#include "miniredis/database.hpp"
#include "miniredis/resp_parser.hpp"
#include "miniredis/presistence_manager.hpp"
#include "miniredis/command_executor.hpp"

using boost::asio::ip::tcp;

class RedisServer {
public:
  RedisServer(boost::asio::io_context& io, int port);

  void run();
  void stop();

private:
  void doAccept();

  boost::asio::io_context& io_;
  tcp::acceptor acceptor_;

  Database db_;
  RespParser parser_;
  PersistenceManager pm_;
  CommandExecutor executor_;
};
