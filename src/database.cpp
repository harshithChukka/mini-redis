#include "miniredis/database.hpp"

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
