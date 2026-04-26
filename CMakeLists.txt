cmake_minimum_required(VERSION 3.16)
project(chat-server VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(SOURCES
    src/main.cpp
    src/config/config.cpp
    src/utils/helper.cpp
    src/command/command_handler.cpp
    src/server/chat_server.cpp
    src/client/client.cpp
    src/network/socket_utils.cpp
)

add_executable(chat_server ${SOURCES})

target_include_directories(chat_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(chat_server PRIVATE -Wall -Wextra -Wpedantic)