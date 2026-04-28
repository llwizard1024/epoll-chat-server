#include <catch2/catch_amalgamated.hpp>

#include "command/command_parser.h"

TEST_CASE("parse plain message", "[parser]") {
    ParsedCommand cmd = parse("Hello, World!");
    REQUIRE(cmd.type == ParsedCommand::PLAIN_MESSAGE);
    REQUIRE(cmd.target.empty());
    REQUIRE(cmd.text == "Hello, World!");
}

TEST_CASE("parse private message", "[parser]") {
    ParsedCommand cmd = parse("/msg Vasya Privet!");
    REQUIRE(cmd.type == ParsedCommand::PRIVATE_MESSAGE);
    REQUIRE(cmd.target == "Vasya");
    REQUIRE(cmd.text == "Privet!");
}

TEST_CASE("parse private message with multiple spaces in text", "[parser]") {
    ParsedCommand cmd = parse("/msg Vasya Hello world!");
    REQUIRE(cmd.type == ParsedCommand::PRIVATE_MESSAGE);
    REQUIRE(cmd.target == "Vasya");
    REQUIRE(cmd.text == "Hello world!");
}

TEST_CASE("parse /msg without text returns plain message fallback", "[parser]") {
    ParsedCommand cmd = parse("/msg Vasya");
    REQUIRE(cmd.type == ParsedCommand::UNKNOWN);
    REQUIRE(cmd.target == "");
    REQUIRE(cmd.text == "");
}

TEST_CASE("parse join room", "[parser]") {
    ParsedCommand cmd = parse("/join cpp_room");
    REQUIRE(cmd.type == ParsedCommand::JOIN_ROOM);
    REQUIRE(cmd.target == "cpp_room");
    REQUIRE(cmd.text.empty());
}

TEST_CASE("parse join room with empty name returns UNKNOWN", "[parser]") {
    ParsedCommand cmd = parse("/join ");
    REQUIRE(cmd.type == ParsedCommand::UNKNOWN);
}

TEST_CASE("parse leave", "[parser]") {
    ParsedCommand cmd = parse("/leave");
    REQUIRE(cmd.type == ParsedCommand::LEAVE_ROOM);
    REQUIRE(cmd.target.empty());
    REQUIRE(cmd.text.empty());
}

TEST_CASE("parse online", "[parser]") {
    ParsedCommand cmd = parse("/online");
    REQUIRE(cmd.type == ParsedCommand::ONLINE_LIST);
    REQUIRE(cmd.target.empty());
    REQUIRE(cmd.text.empty());
}

TEST_CASE("parse unknown command returns plain message", "[parser]") {
    ParsedCommand cmd = parse("/unknown");
    REQUIRE(cmd.type == ParsedCommand::PLAIN_MESSAGE);
    REQUIRE(cmd.text == "/unknown");
}

TEST_CASE("parse empty string returns plain message", "[parser]") {
    ParsedCommand cmd = parse("");
    REQUIRE(cmd.type == ParsedCommand::PLAIN_MESSAGE);
    REQUIRE(cmd.text.empty());
}