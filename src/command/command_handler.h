#pragma once

#include "server/chat_server.h"

#include <string>

struct ParsedCommand {
    enum Type {
        PLAIN_MESSAGE,
        PRIVATE_MESSAGE,
        JOIN_ROOM,
        LEAVE_ROOM,
        ONLINE_LIST,
        ROOM_LIST,
        UNKNOWN
    };

    Type type;
    std::string target; // nickname, room name etc...
    std::string text;
};

class CommandHandler {
    ChatServer& server_;
public:
    explicit CommandHandler(ChatServer& server) : server_(server) {} // for exclude cast Server <-> CommandHandler
    void execute(Client* client, const std::string& line);
private:
    void handle_plain_message(Client* client, const std::string& text);
    void handle_private_message(Client* client, const std::string& target, const std::string& text);
    void handle_join_room(Client* client, const std::string& room_name);
    void handle_leave_room(Client* client);
    void handle_online_list(Client* client);
    void handle_rooms_list(Client* client);
};