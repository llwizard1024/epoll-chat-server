#pragma once

#include <string>

class Room {
    std::string name_;
    std::string password_;
    int user_count_;
public:
    Room(std::string name, std::string password) : name_(name), password_(password), user_count_(0) {};

    std::string getName() const { return name_; }
    int getUserCount() const { return user_count_; };

    void setPassword(std::string password) { password_ = password; }

    bool verification_password(std::string password) const { return password_ == password; }

    void increase_user_counter() { user_count_ += 1; }
    void decrease_user_counter() { user_count_ -= 1; }
};