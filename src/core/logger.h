#pragma once

#include <string>
#include <memory>
#include <spdlog/spdlog.h>


class Logger {
    static std::shared_ptr<spdlog::logger> logger_;
public:
    static void init(const std::string& filename, bool is_daemon);
    static std::shared_ptr<spdlog::logger> get() { return logger_; }
};