#include "Log.h"
#include <iostream>
void liveLog(const std::string &message) {
    static uint32_t lastLogLength = 0;
    std::cout << message;
    if (message.length() < lastLogLength) {
        std::cout << std::string(lastLogLength - message.length(), ' ');
    }
    std::cout << '\r';
    lastLogLength = message.length();
}
