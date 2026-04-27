#include "network/socket_utils.h"

#include "core/logger.h"

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

bool set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return false;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) != -1;
}

int create_server_socket(int port) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        Logger::get()->error("socket: {}", std::strerror(errno));
        return -1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        Logger::get()->error("setsockopt: {}", std::strerror(errno));
        close(server_fd);
        return -1;
    }

    struct sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
        Logger::get()->error("bind: {}", std::strerror(errno));
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, SOMAXCONN) == -1) {
        Logger::get()->error("listen: {}", std::strerror(errno));
        close(server_fd);
        return -1;
    }

    if (!set_nonblocking(server_fd)) {
        Logger::get()->error("set_nonblocking server_fd: {}", std::strerror(errno));
        close(server_fd);
        return -1;
    }

    return server_fd;
}