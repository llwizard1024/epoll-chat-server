#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <unistd.h>

enum ClientState {
    STATE_NICKNAME,
    STATE_CHAT
};

class Client {
public:
    int fd;
    ClientState state;
    std::string nickname;
    std::string in_buffer;
    std::string out_buffer;
    bool want_write;

    Client(int _fd)
        : fd(_fd)
        , state(STATE_NICKNAME)
        , want_write(false)
    {}

    ~Client() { if (fd != -1) close(fd); }

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    void queue_message(const std::string& msg, int epoll_fd);

    bool handle_write(int epoll_fd);
};

#endif