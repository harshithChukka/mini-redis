#include <iostream>
#include <boost/asio.hpp>

#include "miniredis/server.hpp"

constexpr const char* SERVER_PORT = "5555";

#define NUM_THREADS 4

int main() {
  std::cout << "Welcome to Mini-Redis" <<"\n";

  boost::asio::io_context io;
  RedisServer server(io, std::stoi(SERVER_PORT));

  auto guard = boost::asio::make_work_guard(io);
  std::vector<std::thread> threads;

  for (int i = 0; i < NUM_THREADS; i++) {
    threads.emplace_back([&io] {
      io.run();
    });
  }

  server.run();

  for (auto& t : threads) {
    t.join();
  }
}
