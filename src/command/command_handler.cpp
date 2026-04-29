#include "command/command_handler.h"

#include "core/logger.h"
#include "command/command_parser.h"

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
        case ParsedCommand::ROOM_LIST:
            handle_rooms_list(client);
            break;
    }
}

void CommandHandler::handle_plain_message(Client* client, const std::string& text) {
    std::string chat_msg = "[" + client->nickname + "]: " + text + "\n";
    server_.broadcast(chat_msg, client, client->room_);

    
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
    if (client->room_ == room_name) {
        client->queue_message("[SERVER] You are already in this room.\n", server_.epoll_fd_);
        return;
    }

    // Room not found, neet create and set password, after this connect to room
    if (server_.rooms_.count(room_name) == 0) {
        client->state = ClientState::STATE_SETUP_ROOM_PASSWORD;
        client->connected_room_ = room_name;
        client->queue_message("Create room password: ", server_.epoll_fd_);
        return;
    } 
    
    bool room_password_is_empty = server_.rooms_.at(room_name)->verification_password("");

    if (room_password_is_empty) {
        server_.join_room(client, room_name);
        return;
    } else {
        client->state = ClientState::STATE_ENTER_ROOM_PASSWORD;
        client->connected_room_ = room_name;
        client->queue_message("Pls, enter room password: ", server_.epoll_fd_);
        return;
    }
}

void CommandHandler::handle_leave_room(Client* client) {
    if (client->room_ == "lobby") {
        client->queue_message("[SERVER] You are in lobby.\n", server_.epoll_fd_);
        return;
    }

    server_.join_room(client, "lobby");
}

void CommandHandler::handle_online_list(Client* client) {
    std::string list = "Users in room '" + client->room_ + "': ";

    for (const auto& pair : server_.clients_) {
        Client* cl = pair.second.get();
        if (cl->room_ == client->room_) {
            list.append(cl->nickname + " ");
        }
    }

    client->queue_message(list + "\n", server_.epoll_fd_);
}

void CommandHandler::handle_rooms_list(Client* client) {
    std::string list = "Rooms list: ";

    for (const auto& pair : server_.rooms_) {
        Room* room = pair.second.get();
        list.append(room->getName() + "[" + std::to_string(room->getUserCount()) + "] ");
    }

    client->queue_message(list + "\n", server_.epoll_fd_);
}
