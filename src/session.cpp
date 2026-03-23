#include <iostream>
#include <memory>

#include "miniredis/session.hpp"
#include "miniredis/command.hpp"

void Session::start() {
  doRead();
}

void Session::doRead() {
  auto self = shared_from_this();
  boost::asio::async_read_until(
    socket_,
    buffer_,
    "\r\n",
    [self] (const boost::system::error_code& ec,const size_t& len) {
      if (!ec) {
        self->handleCommand(len);
        self->doRead();
      } else self->handleError(ec);
    }
  );
}

void Session::handleCommand(const size_t& /*len*/) {
  std::istream stream(&buffer_);
  std:: string line;

  std::getline(stream, line);

  if (!line.empty() && line.back() == '\r')
    line.pop_back();

  Command cmd = parser_.parse(line);
  std::string result = executor_.execute(cmd);
  doWrite(result);
}

void Session::doWrite(const std::string& msg) {
  bool shouldStart = false;
  {
    std::lock_guard lock(writeMutex_);
    writeQueue_.push_back(msg);
    if (!isWriteInProgress) {
      isWriteInProgress = true;
      shouldStart = true;
    }
  }
  if (shouldStart) writeNext();
}

bool Session::tryPrepareNextMessage(std::shared_ptr<std::string>& nextMessage) {
  std::lock_guard lock(writeMutex_);
  const bool cannotWrite = !socket_.is_open() || writeQueue_.empty();
  if (cannotWrite) {
    isWriteInProgress = false;
    return false;
  }

  nextMessage = std::make_shared<std::string>(writeQueue_.front());
  return true;
}

void Session::handleWriteComplete(const boost::system::error_code& ec) {
  if (ec) {
    handleError(ec);
    return;
  }
  {
    std::lock_guard lock(writeMutex_);
    writeQueue_.pop_front();
  }
  writeNext();
}

void Session::writeNext() {
  std::shared_ptr<std::string> nextMessage;
  if (!tryPrepareNextMessage(nextMessage)) return;

  auto self = shared_from_this();
  boost::asio::async_write(
    socket_,
    boost::asio::buffer(*nextMessage),
    [self, nextMessage] (const boost::system::error_code& ec, size_t /*bytes*/) {
      self->handleWriteComplete(ec);
    }
  );
}

void Session::handleError(const boost::system::error_code& ec) {
  std::cerr << "Error during Async I/O: " << ec.message() << "\n";
  {
    std::lock_guard lock(writeMutex_);
    writeQueue_.clear();
    isWriteInProgress = false;
  }
  boost::system::error_code ignored;
  if (socket_.is_open()) {
    socket_.shutdown(boost::asio::socket_base::shutdown_both, ignored);
    socket_.close(ignored);
  }
}
