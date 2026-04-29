![Build Status](https://github.com/llwizard1024/epoll-chat-server/actions/workflows/build_and_test.yml/badge.svg)
# epoll-chat-server 🚀

An asynchronous multi-user TCP chat server written in modern C++ using epoll, non-blocking sockets, and a simple text-based protocol. It supports chat rooms, private messages, daemonization, and file logging.

## ✨ Features

- **Non-blocking I/O multiplexing** with epoll (Edge-Triggered) in a single thread
- **Chat rooms** – create, join, leave, list participants
- **Private messages** – direct communication between users
- **Daemonization** – run in the background, detach from the terminal
- **File logging** based on spdlog with immediate flush
- **Graceful shutdown** on SIGTERM / SIGINT
- **Configuration via .env file** in KEY=VALUE format

## 📦 Build

Requirements:
- CMake ≥ 3.16
- C++20 compiler (GCC ≥ 9, Clang ≥ 10)
- spdlog (included as header-only)

Build:

```bash
git clone https://github.com/llwizard1024/epoll-chat-server.git
cd epoll-chat-server
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
## ⚙️ Configuration

Settings are stored in the server.env file in the project root.
```bash
cp server.env.example server.env
```
Read .env file for detail info

When DAEMON=true the server detaches from the terminal, closes stdin/stdout/stderr and continues running in the background.

## 🚀 Usage

Normal mode:
```bash
./chat_server
```
Daemon mode (set DAEMON=true in server.env):
```bash
./chat_server
# Shell returns immediately, server runs in background
```

Stopping the daemon:
```bash
kill -TERM $(pgrep chat_server)
```

## 💬 Client commands

### Connect with any TCP client (telnet, netcat):
```bash
nc localhost 8080
```

| Command  | Description |
| ------------- | ------------- |
| any text  | Send a message to the current room |
| /msg nickname text  | Send a private message to a specific user |
| /join room_name | Join or create a room |
|/leave	     | Leave the current room and return to the lobby |
|/online     | Show participants in the current room |

# 🛠️ Project structure

```
src/
├── main.cpp                  # Entry point, config loading, server startup
├── server/
│   ├── chat_server.h/cpp     # Main server class and epoll loop
├── client/
│   ├── client.h/cpp          # Client: non-blocking I/O, buffers
├── command/
│   ├── command_handler.h/cpp # Command parsing and execution (/msg, /join, …)
├── network/
│   ├── socket_utils.h/cpp    # Socket creation, configuration, set_nonblocking
├── config/
│   ├── config.h/cpp          # .env file reader
├── core/
│   ├── logger.h/cpp          # spdlog wrapper (file + console sinks)
│   ├── daemon.h/cpp          # Daemonization (fork, setsid, /dev/null redirection)
|── utils/
|   |── helper.h/cpp          # Helper functions for formating text and etc...
```
