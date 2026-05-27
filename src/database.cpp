#include "miniredis/database.hpp"

namespace {

std::optional<int64_t> convertToInt(const std::string& s) {
  try {
    size_t pos;
    int64_t value = std::stoll(s, &pos);
    if (pos == s.length()) 
      return value;
    return std::nullopt;
  }
  catch (...) {
    return  std::nullopt;
  }
}

}

void Database::set(const std::string& key, const std::string& value) {
  std::lock_guard<std::mutex> lock(mutex_);
  store_[key] = value;
  expiry_.erase(key);
}

std::shared_ptr<std::string> Database::get(const std::string& key) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = store_.find(key);
  auto itExp = expiry_.find(key);

  if (it == store_.end())
    return nullptr;

  if (itExp != expiry_.end() && std::chrono::steady_clock::now() > itExp->second) {
    store_.erase(it);
    expiry_.erase(itExp);
    return nullptr;
  }

  auto value = std::make_shared<std::string>(it->second);  
  return value;
}

int Database::del(const std::string& key) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = store_.find(key);
  if (it == store_.end())
    return 0;
  store_.erase(it);
  expiry_.erase(key);
  return 1;
}

std::optional<int64_t> Database::incr(const std::string& key) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = store_.find(key);
  if (it == store_.end()) {
    store_[key] = std::to_string(1);
    return 1;
  }

  auto value = convertToInt(it->second);
  if (!value)
    return std::nullopt;

  ++(*value);
  it->second = std::to_string(*value);
  return *value;
}

int Database::expire(const std::string& key, const std::string& sec) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = store_.find(key);
  auto value = convertToInt(sec);
  if (it == store_.end() || !value || *value < 0)
    return 0;

  expiry_[key] = std::chrono::steady_clock::now() + std::chrono::seconds(*value);
  return 1;
}
