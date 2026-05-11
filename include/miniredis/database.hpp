#pragma once

#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <memory>
#include <optional>

class Database {
public:
    void set(const std::string& key, const std::string& value);
    std::shared_ptr<std::string> get(const std::string& key);
    int del(const std::string& key);
    std::optional<int64_t> incr(const std::string& key);

private:
    std::unordered_map<std::string, std::string> store_;
    std::shared_mutex mutex_;
};
