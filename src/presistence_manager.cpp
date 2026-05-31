#include <boost/asio/streambuf.hpp>
#include "miniredis/presistence_manager.hpp"
#include "miniredis/command_executor.hpp"

void PersistenceManager::load(CommandExecutor& executor) {
  std::ifstream file(AOF_FILE);
  if (!file.is_open()) {
    std::cerr << "[persistence_manager] No AOF file found, starting fresh.\n";
    return;
  }

  boost::asio::streambuf buf;
  std::ostream os(&buf);
  os << file.rdbuf();

  int replayed = 0;
  while (true) {
    auto cmd = parser_.parse(buf);
    if (!cmd) break;
    executor.execute(cmd.value(), true);
    ++replayed;
  }
  std::cout << "[persistence_manager] Replayed " << replayed << " commands from AOF.\n";
}

void PersistenceManager::append(const std::string& data) {
  if (aof_writer_.is_open()) {
    aof_writer_ << data;
    aof_writer_.flush();
  } else {
    std::cerr << "[persistence_manager] AOF write stream is closed.\n";
  }
}
