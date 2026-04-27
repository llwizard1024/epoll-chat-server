#include "server/chat_server.h"

#include "network/socket_utils.h"
#include "command/command_handler.h"
#include "core/logger.h"

#include <sys/epoll.h>
#include <arpa/inet.h>

void ChatServer::run() {
    server_fd_ = create_server_socket(port_);
    if (server_fd_ == -1) {
        Logger::get()->error("server socket create: {}", std::strerror(errno));
        return;
    }

    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ == -1) {
        Logger::get()->error("epoll_create1: {}", std::strerror(errno));
        close(server_fd_);
        return;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = server_fd_;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, server_fd_, &ev) == -1) {
        Logger::get()->error("epoll_ctl ADD server_fd: {}", std::strerror(errno));
        close(server_fd_);
        close(epoll_fd_);
        return;
    }

    Logger::get()->info("[SERVER] Server is running on the port - {}", port_);
    Logger::get()->info("[SERVER] Waiting for connections...");

    struct epoll_event events[ChatServer::MAX_EVENTS];

    while (true) {
        int nfds = epoll_wait(epoll_fd_, events, ChatServer::MAX_EVENTS, -1);
        if (nfds == -1) {
            Logger::get()->error("epoll_wait: {}", std::strerror(errno));
            break;
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == server_fd_) {
                accept_new_client();
                continue;
            }

            Client* client = static_cast<Client*>(events[i].data.ptr);
            if (!client) continue;

            if (events[i].events & (EPOLLERR | EPOLLHUP)) {
                disconnect_client(client);
                continue;
            }

            if (events[i].events & EPOLLIN) {
                handle_client_read(client);
            }

            if (events[i].events & EPOLLOUT) {
                handle_client_write(client);
            }
        }
    }
}

void ChatServer::stop() {
    return;
}

void ChatServer::accept_new_client() {
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, &addr_len);

        if (client_fd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                Logger::get()->error("accept: {}", std::strerror(errno));
                break;
            }
        }

        if (!set_nonblocking(client_fd)) {
            Logger::get()->error("set_nonblocking client_fd: {}", std::strerror(errno));
            close(client_fd);
            continue;
        }

        clients_[client_fd] = std::make_unique<Client>(client_fd);
        Client* raw_ptr = clients_[client_fd].get();

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

        Logger::get()->info("[SERVER] New connection: {} : {} (fd={})", 
            client_ip, ntohs(client_addr.sin_port), client_fd);

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.ptr = raw_ptr;
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
            Logger::get()->error("epoll_ctl ADD client: {}", std::strerror(errno));
            clients_.erase(client_fd);
        } else {
            std::string prompt = "Enter your nickname:\n";
            raw_ptr->queue_message(prompt, epoll_fd_);
        }
    }
}

void ChatServer::handle_client_read(Client* client) {
    char buffer[ChatServer::BUFFER_SIZE];
    while (true) {
        ssize_t n = read(client->fd, buffer, sizeof(buffer) - 1);
        if (n == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                Logger::get()->error("read: {}", std::strerror(errno));
                disconnect_client(client);
                return;
            }
        }
        if (n == 0) {
            disconnect_client(client);
            return;
        }

        client->in_buffer.append(buffer, n);

        size_t pos;
        while ((pos = client->in_buffer.find('\n')) != std::string::npos) {
            std::string line = client->in_buffer.substr(0, pos);
            client->in_buffer.erase(0, pos + 1);

            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }

            if (line.empty()) continue;

            process_message(client, line);
        }
    }
}
void ChatServer::handle_client_write(Client* client) {
    if (!client->handle_write(epoll_fd_)) {
        disconnect_client(client);
    }
}

void ChatServer::disconnect_client(Client* client) {
    if (!client) return;

    Logger::get()->info("[SERVER] Client disconnected: fd={}", client->fd);

    if (!client->nickname.empty()) {
        send_disconnect_notification(client->nickname);
    }

    epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, client->fd, nullptr);
    close(client->fd);
    clients_.erase(client->fd);
}

void ChatServer::broadcast(const std::string& msg, Client* sender, const std::string& room_name) {
    for (auto& pair : clients_) {
        Client* cl = pair.second.get();
        if (cl->state != ClientState::STATE_CHAT) continue;

        if (room_name.empty() || cl->room == room_name) {
            cl->queue_message(msg, epoll_fd_);
        }
    }
}

void ChatServer::send_disconnect_notification(const std::string& nickname) {
    for (auto& pair : clients_) {
        Client* cl = pair.second.get();
        if (cl->state == ClientState::STATE_CHAT) {
            cl->queue_message(nickname + " left the server\n", epoll_fd_);
        }
    }
}

void ChatServer::process_message(Client* client, const std::string& line) {
    //setup nickname user
    if (client->state == ClientState::STATE_NICKNAME) {
        client->nickname = line;
        client->state = ClientState::STATE_CHAT;

        std::string welcome = "Welcome to the chat, " + client->nickname + "!\n";
        client->queue_message(welcome, epoll_fd_);

        std::string join_msg = "[SERVER]: " + client->nickname + " joined and entered the lobby.\n";
        broadcast(join_msg, client);

        Logger::get()->info("[SERVER] Client fd={} set a nickanem {}", client->fd, client->nickname);
        return;
    }

    CommandHandler handler(*this);
    handler.execute(client, line);
}