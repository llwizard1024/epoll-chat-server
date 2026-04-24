#include "command/command_handler.h"

#include <iostream>

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
    }
}

void CommandHandler::handle_plain_message(Client* client, const std::string& text) {
    std::string chat_msg = "[" + client->nickname + "]: " + text + "\n";
    server_.broadcast(chat_msg, client);

    //Server log
    std::cout << "[SERVER] Сообщение от " << client->nickname << ": " << text << std::endl;
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
    client->queue_message("User: " + target + " not found", server_.epoll_fd_);
}
