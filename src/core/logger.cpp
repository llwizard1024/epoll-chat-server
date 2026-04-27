#include "core/logger.h"

#include <vector>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Logger::logger_;

void Logger::init(const std::string& filename, bool is_daemon) {
    std::vector<spdlog::sink_ptr> sinks;

    if (is_daemon == false) {
        sinks.push_back(std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>());
    }

    sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, true));

    auto logger = std::make_shared<spdlog::logger>("chat_server", sinks.begin(), sinks.end());

    logger->set_level(spdlog::level::info);

    logger->info("Logger initialized.");
    logger->flush();

    logger->flush_on(spdlog::level::info);
    logger->flush_on(spdlog::level::err);

    logger_ = logger;
}