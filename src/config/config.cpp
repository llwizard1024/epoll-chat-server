#include "config/config.h"

#include "utils/helper.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

bool Config::load(const std::string& file_name) {
    std::ifstream file(file_name);

    if(!file.is_open()) return false;
    
    std::string line;

    while(std::getline(file, line)) {
        if (line.empty()) continue;
        
        parse_line(line);
    }

    file.close();
    return true;

}

std::string Config::get(const std::string& key, const std::string& default_value) const {
    auto it = data_.find(key);
   
    if (it != data_.end()) {
        return it->second;
    }
    
    return default_value;
}

void Config::parse_line(const std::string& line) {
    if (line.empty()) return;
    
    size_t hash_sign_index = line.find("#");

    if (hash_sign_index == 0) return; // start at #, comment line

    size_t equal_sign_index = 0;

    for(size_t i = 0; i < line.size(); ++i) {
        if (line[i] == '=') {
            equal_sign_index = i;
            break;
        }
    }

    if (equal_sign_index == 0 || equal_sign_index == line.size() - 1) return; // = or key=
    if (equal_sign_index > hash_sign_index) return; // key#=value

    std::string key = line.substr(0, equal_sign_index);
    std::string value = line.substr(equal_sign_index + 1);

    if (hash_sign_index != std::string::npos) {
        size_t hash_index_in_value = value.find("#");
        value.erase(hash_index_in_value);
    }

    trim_string(key);
    trim_string(value);

    data_[key] = value;
}