#pragma once

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class RedisServer {
public:
  RedisServer(boost::asio::io_context& io, int port) : io_(io), acceptor_(io, tcp::endpoint(tcp::v4(), port)) {};

  void run();
  void stop();

private:
  void doAccept();

  boost::asio::io_context& io_;
  tcp::acceptor acceptor_;
};
