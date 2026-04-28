#include <catch2/catch_amalgamated.hpp>

#include "config/config.h"

#include <fstream>
#include <string>

class TempEnvFile {
    std::ofstream file_;
    std::string file_name_;
public:
    TempEnvFile(const std::string& file_name, const std::string& data) {
        file_.open(file_name);
        file_name_ = file_name;

        file_ << data;
        file_.close();
    }

    ~TempEnvFile() {
        std::remove(file_name_.c_str());
    }

    TempEnvFile(const TempEnvFile& other) = delete;
    TempEnvFile(const TempEnvFile&& other) = delete;
    TempEnvFile& operator=(TempEnvFile&& other) = delete;

    std::string get_file_name() const {
        return file_name_;
    }
};

// basic
TEST_CASE("Key - Value case", "[config][basic]") {
    TempEnvFile env_file("test.env", "PORT=1337");

    Config cfg;
    cfg.load(env_file.get_file_name());

    REQUIRE(cfg.get("PORT") == "1337");
}

TEST_CASE("Default value case", "[config][basic]") {
    TempEnvFile env_file("test.env", "PORT=1984");

    Config cfg;
    cfg.load(env_file.get_file_name());

    REQUIRE(cfg.get("NONEXSISTS", "default") == "default");
}

TEST_CASE("Load config return false case", "[config][basic]") {
    Config cfg;
    REQUIRE(cfg.load("abobus.env") == false);
}

// whitespace
TEST_CASE("Whitspace in key and value case", "[config][whitespace]") {
    TempEnvFile env_file("test.env", " PORT = 1984 ");

    Config cfg;
    cfg.load(env_file.get_file_name());

    REQUIRE(cfg.get("PORT") == "1984");
}

TEST_CASE("Only whitespace in file case", "[config][whitespace]") {
    TempEnvFile env_file("test.env", " ");

    Config cfg;
    cfg.load(env_file.get_file_name());

    REQUIRE(cfg.get("PORT", "1984") == "1984");
}

// comments
TEST_CASE("Basic comment case", "[config][comments]") {
    TempEnvFile env_file("test.env", "#PORT=1994");

    Config cfg;
    cfg.load(env_file.get_file_name());

    REQUIRE(cfg.get("PORT", "1000") == "1000");
}

TEST_CASE("Comment after value case", "[config][comments]") {
    TempEnvFile env_file("test.env", "PORT=1234 # Its comment :)");

    Config cfg;
    cfg.load(env_file.get_file_name());

    REQUIRE(cfg.get("PORT") == "1234");
}

TEST_CASE("Few comments case", "[config][comments]") {
    TempEnvFile env_file("test.env", "#123\nPORT=1234\n#4321\nDAEMON=true");

    Config cfg;
    cfg.load(env_file.get_file_name());

    REQUIRE(cfg.get("PORT") == "1234");
    REQUIRE(cfg.get("DAEMON") == "true");
}

// invalid
TEST_CASE("Key without value case", "[config][invalid]") {
    TempEnvFile env_file("test.env", "PORT");

    Config cfg;
    cfg.load(env_file.get_file_name());

    REQUIRE(cfg.get("PORT", "1234") == "1234");
}

TEST_CASE("Value without key case", "[config][invalid]") {
    TempEnvFile env_file("test.env", "=1234");

    Config cfg;
    cfg.load(env_file.get_file_name());

    REQUIRE(cfg.get("PORT", "4321") == "4321");
}

TEST_CASE("Key and equal sign without value case", "[config][invalid]") {
    TempEnvFile env_file("test.env", "PORT=");

    Config cfg;
    cfg.load(env_file.get_file_name());

    REQUIRE(cfg.get("PORT", "1234") == "1234");
}

// edge_cases
TEST_CASE("Few equal sign case", "[config][edge_cases]") {
    TempEnvFile env_file("test.env", "KEY=VAL=UE");

    Config cfg;
    cfg.load(env_file.get_file_name());

    REQUIRE(cfg.get("KEY") == "VAL=UE");
}

TEST_CASE("Two equal keys case", "[config][edge_cases]") {
    TempEnvFile env_file("test.env", "PORT=222\nPORT=333");

    Config cfg;
    cfg.load(env_file.get_file_name());

    REQUIRE(cfg.get("PORT") == "333");
}

TEST_CASE("Whitespaces in value saved case", "[config][edge_cases]") {
    TempEnvFile env_file("test.env", "PORT=Hello World");

    Config cfg;
    cfg.load(env_file.get_file_name());

    REQUIRE(cfg.get("PORT") == "Hello World");
}