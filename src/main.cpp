#include "server/chat_server.h"
#include "config/config.h"

#include <spdlog/spdlog.h>

int main() {
    Config config;
    config.load("server.env");

    int port = std::stoi(config.get("PORT", "8080"));

    ChatServer server(port);
    server.run();

    return 0;
}