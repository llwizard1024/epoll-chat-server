#include "command/command_handler.h"

#include "core/logger.h"

ParsedCommand CommandHandler::parse(const std::string& line) const {
    if (line.rfind("/msg ", 0) == 0) {
        size_t index_nickname_start = 0;
    
        for (size_t i = 5; i < line.size(); ++i) {
            if (line[i] == ' ') {
                index_nickname_start = i;
                break;
            }
        }
        
        if (index_nickname_start == 0) {
            return ParsedCommand{ParsedCommand::PLAIN_MESSAGE, "", line};
        }
        
        std::string nickname = line.substr(5, index_nickname_start - 5);
        std::string msg = line.substr(index_nickname_start + 1);

        return ParsedCommand{ParsedCommand::PRIVATE_MESSAGE, nickname, msg};
    } else if (line.rfind("/join ", 0) == 0) {
        std::string room_name = line.substr(6);

        if (room_name.empty()) {
            return ParsedCommand{ParsedCommand::UNKNOWN, "", ""};
        }

        return ParsedCommand{ParsedCommand::JOIN_ROOM, room_name, ""};
    } else if (line.rfind("/leave", 0) == 0) {
        return ParsedCommand{ParsedCommand::LEAVE_ROOM, "", ""};
    } else if (line.rfind("/online", 0) == 0) {
        return ParsedCommand{ParsedCommand::ONLINE_LIST, "", ""};
    }
    
    return ParsedCommand{ParsedCommand::PLAIN_MESSAGE, "", line};
}

void CommandHandler::execute(Client* client, const std::string& line) {
    ParsedCommand cmd = parse(line);

    switch(cmd.type) {
        case ParsedCommand::PRIVATE_MESSAGE:
            handle_private_message(client, cmd.target, cmd.text);
            break;
        case ParsedCommand::PLAIN_MESSAGE:
            handle_plain_message(client, cmd.text);
            break;
        case ParsedCommand::JOIN_ROOM:
            handle_join_room(client, cmd.target);
            break;
        case ParsedCommand::LEAVE_ROOM:
            handle_leave_room(client);
            break;
        case ParsedCommand::ONLINE_LIST:
            handle_online_list(client);
            break;
    }
}

void CommandHandler::handle_plain_message(Client* client, const std::string& text) {
    std::string chat_msg = "[" + client->nickname + "]: " + text + "\n";
    server_.broadcast(chat_msg, client, client->room);

    
    Logger::get()->info("[SERVER] Сообщение от {} : {}", client->nickname, text);
}

void CommandHandler::handle_private_message(Client* client, const std::string& target, const std::string& text) {
    for (const auto& pair : server_.clients_) {
        Client* cl = pair.second.get();
        if (cl->nickname == target && cl->state == ClientState::STATE_CHAT) {
            std::string chat_msg_from = "[Private msg from " + client->nickname + "]: " + text + "\n";
            cl->queue_message(chat_msg_from, server_.epoll_fd_);
            
            std::string chat_msg_to = "[Private msg to " + cl->nickname + "]: " + text + "\n";
            client->queue_message(chat_msg_to, server_.epoll_fd_);
            return;
        }
    }

    // Client not found with this nickname, send error
    client->queue_message("User: " + target + " not found\n", server_.epoll_fd_);
}

void CommandHandler::handle_join_room(Client* client, const std::string& room_name) {
    if (client->room == room_name) {
        client->queue_message("[SERVER] You are already in this room.\n", server_.epoll_fd_);
        return;
    }

    std::string old_room_name = client->room;
    client->room = room_name;

    std::string msg_quit = "[SERVER]: " + client->nickname + " left the room.\n";
    server_.broadcast(msg_quit, client, old_room_name);

    std::string msg_enter = "[SERVER]: " + client->nickname + " joined the room.\n";
    server_.broadcast(msg_enter, client, room_name);
}

void CommandHandler::handle_leave_room(Client* client) {
    if (client->room == "lobby") {
        client->queue_message("[SERVER] You are in lobby.\n", server_.epoll_fd_);
        return;
    }

    std::string old_room_name = client->room;
    client->room = "lobby";

    std::string msg_quit = "[SERVER]: " + client->nickname + " left the room.\n";
    server_.broadcast(msg_quit, client, old_room_name);

    std::string msg_enter = "[SERVER]: " + client->nickname + " joined the lobby.\n";
    server_.broadcast(msg_enter, client, "lobby");
}

void CommandHandler::handle_online_list(Client* client) {
    std::string list = "Users in room '" + client->room + "': ";

    for (const auto& pair : server_.clients_) {
        Client* cl = pair.second.get();
        if (cl->room == client->room) {
            list.append(cl->nickname + " ");
        }
    }

    client->queue_message(list + "\n", server_.epoll_fd_);
}
