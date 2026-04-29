#pragma once

#include <string>
#include <unistd.h>

#include "room/room.h"

enum class ClientState {
    STATE_NICKNAME,
    STATE_CHAT,
    STATE_SETUP_ROOM_PASSWORD,
    STATE_ENTER_ROOM_PASSWORD
};

class Client {
public:
    int fd;
    ClientState state;
    std::string nickname;
    std::string in_buffer;
    std::string out_buffer;
    bool want_write;
    std::string room_;
    std::string connected_room_;

    Client(int _fd)
        : fd(_fd)
        , state(ClientState::STATE_NICKNAME)
        , want_write(false)
        , room_("")
        , connected_room_("")
    {}

    ~Client() { if (fd != -1) close(fd); }

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    void queue_message(const std::string& msg, int epoll_fd);
    bool handle_write(int epoll_fd);
};