cmake_minimum_required(VERSION 3.16)
project(chat-server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(chat_lib STATIC
    src/config/config.cpp
    src/core/logger.cpp
    src/core/daemon.cpp
    src/utils/helper.cpp
    src/command/command_handler.cpp
    src/command/command_parser.cpp
    src/server/chat_server.cpp
    src/client/client.cpp
    src/room/room.cpp
    src/network/socket_utils.cpp
)

target_include_directories(chat_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(chat_lib PRIVATE -Wall -Wextra -Wpedantic)

add_executable(chat_server src/main.cpp)
target_link_libraries(chat_server PRIVATE chat_lib)


enable_testing()

add_executable(chat_tests
    external/catch2/catch_amalgamated.cpp
    tests/test_main.cpp
    tests/test_config.cpp
    tests/test_command_parser.cpp
    tests/test_utils.cpp
)

target_link_libraries(chat_tests PRIVATE chat_lib)
target_include_directories(chat_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external
)