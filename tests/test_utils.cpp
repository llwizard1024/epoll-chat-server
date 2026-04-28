#include <catch2/catch_amalgamated.hpp>
#include "utils/helper.h"

TEST_CASE("trim empty string", "[utils]") {
    std::string s;
    trim_string(s);
    REQUIRE(s.empty());
}

TEST_CASE("trim string with no spaces", "[utils]") {
    std::string s = "hello";
    trim_string(s);
    REQUIRE(s == "hello");
}

TEST_CASE("trim string with leading spaces", "[utils]") {
    std::string s = "   hello";
    trim_string(s);
    REQUIRE(s == "hello");
}

TEST_CASE("trim string with trailing spaces", "[utils]") {
    std::string s = "hello   ";
    trim_string(s);
    REQUIRE(s == "hello");
}

TEST_CASE("trim string with both leading and trailing spaces", "[utils]") {
    std::string s = "   hello world   ";
    trim_string(s);
    REQUIRE(s == "hello world");
}

TEST_CASE("trim string with only spaces", "[utils]") {
    std::string s = "     ";
    trim_string(s);
    REQUIRE(s.empty());
}

TEST_CASE("trim string with single space", "[utils]") {
    std::string s = " ";
    trim_string(s);
    REQUIRE(s.empty());
}

TEST_CASE("trim string with internal spaces preserved", "[utils]") {
    std::string s = "  hello  world  ";
    trim_string(s);
    REQUIRE(s == "hello  world");
}