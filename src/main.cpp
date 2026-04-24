#include "server/chat_server.h"

int main(int argc, char* argv[]) {
    int port = 8080;

    if (argc > 1) {
        port = std::atoi(argv[1]);
    }

    ChatServer server(port);
    server.run();

    return 0;
}