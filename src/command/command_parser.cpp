#include "command/command_parser.h"

ParsedCommand parse(const std::string& line) {
    if (line.rfind("/msg ", 0) == 0) {
        size_t index_nickname_start = 0;
        for (size_t i = 5; i < line.size(); ++i) {
            if (line[i] == ' ') {
                index_nickname_start = i;
                break;
            }
        }

        if (index_nickname_start == 0 || index_nickname_start == line.size() - 1) {
            return ParsedCommand{ParsedCommand::UNKNOWN, "", ""};
        }
        
        std::string nickname = line.substr(5, index_nickname_start - 5);
        std::string msg = line.substr(index_nickname_start + 1);
        return ParsedCommand{ParsedCommand::PRIVATE_MESSAGE, nickname, msg};
    } else if (line.rfind("/join ", 0) == 0) {
        std::string room_name = line.substr(6);

        if (room_name.empty()) {
            return ParsedCommand{ParsedCommand::UNKNOWN, "", ""};
        }

        return ParsedCommand{ParsedCommand::JOIN_ROOM, room_name, ""};
    } else if (line.rfind("/leave", 0) == 0) {
        return ParsedCommand{ParsedCommand::LEAVE_ROOM, "", ""};
    } else if (line.rfind("/online", 0) == 0) {
        return ParsedCommand{ParsedCommand::ONLINE_LIST, "", ""};
    }
    
    return ParsedCommand{ParsedCommand::PLAIN_MESSAGE, "", line};
}