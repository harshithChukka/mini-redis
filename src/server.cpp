#include <iostream>

#include "miniredis/server.hpp"
#include "miniredis/session.hpp"

RedisServer::RedisServer(boost::asio::io_context& io, int port)
  : io_(io)
  , acceptor_(io, tcp::endpoint(tcp::v4(), port))
  , pm_(db_, parser_)
  , executor_(db_, pm_) {
  pm_.load(executor_);
}

void RedisServer::run() {
  doAccept();
}

void RedisServer::doAccept() {
  std::cout << "Listening ..." << "\n";
  auto session = std::make_shared<Session>(io_, executor_);

  acceptor_.async_accept(session->socket(), [this, session] (const boost::system::error_code& ec) {
    if (!ec) {
      std::cout << "A new client connected.\n";
      session->start();
    }
    doAccept();
  });
}

void RedisServer::stop() {
  acceptor_.close();
}
