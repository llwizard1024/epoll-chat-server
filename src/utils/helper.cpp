#include "utils/helper.h"

void trim_string(std::string& str) {
    if (str.empty()) {
        return;
    }

    int start_index = 0;

    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == ' ') {
            start_index++;
        } else {
            break;
        }
    }

    str.erase(0, start_index);

    if (str.empty()) return;

    int end_index = str.size();
    
    for (size_t i = str.size() - 1; i > 0; --i) {
        if (str[i] == ' ') {
            end_index--;
        } else {
            break;
        }
    }

    str.erase(end_index);
}