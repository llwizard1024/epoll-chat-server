#include "client/client.h"

bool Client::handle_write(int epoll_fd) {
    if (out_buffer.empty()) {
        if (want_write) {
            want_write = false;
            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLET;
            ev.data.ptr = this;
            if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
                perror("epoll_ctl MOD (remove EPOLLOUT)");
            }
        }
        return true;
    }

    ssize_t written = write(fd, out_buffer.c_str(), out_buffer.size());
    if (written == -1) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            return false;
        }
        return false;
    }

    out_buffer.erase(0, written);

    if (out_buffer.empty() && want_write) {
        want_write = false;
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.ptr = this;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
            perror("epoll_ctl MOD (remove EPOLLOUT)");
        }
    }
    return true;
}

void Client::queue_message(const std::string& msg, int epoll_fd) {
    out_buffer += msg;
    if (!want_write && !out_buffer.empty()) {
        want_write = true;
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
        ev.data.ptr = this;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &ev) == -1) {
            perror("epoll_ctl MOD (add EPOLLOUT)");
        }
    }
}