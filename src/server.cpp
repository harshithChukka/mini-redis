#include <iostream>

#include "miniredis/server.hpp"
#include "miniredis/session.hpp"

void RedisServer::run() {
  doAccept();
}

void RedisServer::doAccept() {
  std::cout << "Listening ..." << "\n";
  auto session = std::make_shared<Session>(io_);

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
