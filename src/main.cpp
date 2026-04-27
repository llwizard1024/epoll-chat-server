#include "server/chat_server.h"
#include "config/config.h"
#include "core/logger.h"
#include "core/daemon.h"

#include <spdlog/spdlog.h>
#include <csignal>

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int signal_number) {
    keep_running = 0;
}

int main() {
    Config config;
    config.load("server.env");

    int port = std::stoi(config.get("PORT", "8080"));
    std::string log_file_name = config.get("LOG_FILE", "server.log");
    bool daemon_enabled = config.get("DAEMON", "false") == "true" ? true : false;

    Logger::init(log_file_name, daemon_enabled);

    if (daemon_enabled) {
        daemonize();
        struct sigaction sa;
        std::memset(&sa, 0, sizeof(sa));

        sa.sa_handler = handle_sigint;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        sigaction(SIGTERM, &sa, nullptr);
        sigaction(SIGINT, &sa, nullptr);
    }

    ChatServer server(port);
    server.run();

    return 0;
}