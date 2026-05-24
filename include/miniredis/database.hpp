#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <optional>

class Database {
public:
    void set(const std::string& key, const std::string& value);
    std::shared_ptr<std::string> get(const std::string& key);
    int del(const std::string& key);
    std::optional<int64_t> incr(const std::string& key);
    int expire(const std::string& key, const std::string& sec);

private:
    std::unordered_map<std::string, std::string> store_;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> expiry_;
    std::mutex mutex_;
};
