#pragma once

#include <string>
#include <unordered_map>

class Config {
    std::unordered_map<std::string, std::string> data_;
public:
    bool load(const std::string& file_name);
    std::string get(const std::string& key, const std::string& default_value = "") const;

private:
    void parse_line(const std::string& line);
};