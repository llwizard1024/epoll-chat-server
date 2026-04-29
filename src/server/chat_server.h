#pragma once

#include "client/client.h"
#include "room/room.h"

#include <memory>
#include <unordered_map>

class ChatServer {
    int server_fd_;
    int epoll_fd_;
    int port_;
    std::unordered_map<int, std::unique_ptr<Client>> clients_;
    std::unordered_map<std::string, std::unique_ptr<Room>> rooms_;

    friend class CommandHandler;
public:
    static constexpr const int MAX_EVENTS = 64;
    static constexpr const int BUFFER_SIZE = 1024;
    
    ChatServer(int port) : server_fd_(-1), epoll_fd_(-1), port_(port)
    {}

    ~ChatServer() {
        if (server_fd_ != -1) close(server_fd_);
        if (epoll_fd_ != -1) close(epoll_fd_);

        clients_.clear();
    }

    void run();
    void stop();
private:
    void accept_new_client();
    void handle_client_read(Client* client);
    void handle_client_write(Client* client);
    void disconnect_client(Client* client);
    void broadcast(const std::string& msg, Client* sender = nullptr, const std::string& room_name = "");
    void send_disconnect_notification(const std::string& nickname);
    void process_message(Client* client, const std::string& line);

    void join_room(Client* client, const std::string& new_room);
    void leave_room(Client* client);
};