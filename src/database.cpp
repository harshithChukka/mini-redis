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
  std::lock_guard<std::shared_mutex> lock(mutex_);
  store_[key] = value;
}

std::shared_ptr<std::string> Database::get(const std::string& key) {
  std::shared_lock<std::shared_mutex> lock(mutex_);
  auto it = store_.find(key);
  if (it == store_.end())
    return nullptr;

  auto value = std::make_shared<std::string>(it->second);  
  return value;
}

int Database::del(const std::string& key) {
  std::lock_guard<std::shared_mutex> lock(mutex_);
  if (store_.find(key) == store_.end())
    return 0;
  store_.erase(key);
  return 1;
}

std::optional<int64_t> Database::incr(const std::string& key) {
  std::lock_guard<std::shared_mutex> lock(mutex_);
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
