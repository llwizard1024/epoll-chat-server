#include "server/chat_server.h"
#include "config/config.h"
#include "core/logger.h"

#include <spdlog/spdlog.h>

int main() {
    Config config;
    config.load("server.env");

    int port = std::stoi(config.get("PORT", "8080"));
    std::string log_file_name = config.get("LOG_FILE", "server.log");

    Logger::init(log_file_name, true);

    ChatServer server(port);
    server.run();

    return 0;
}