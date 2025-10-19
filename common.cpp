#include "common.h"
#include <cctype>
#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

int readString(int fd, std::string &line) {
    char symbol;
    line.clear();
    while (true) {
        int res;
#ifdef _WIN32
        res = _read(fd, &symbol, sizeof(char));
#else
        res = read(fd, &symbol, sizeof(char));
#endif
        if (res <= 0) {
            return EOF;
        }
        
        if (symbol == '\n') {
            break;
        }
        
        line.push_back(symbol);
    }

    return line.length();
}

void writeString(int fd, const std::string &line) {
#ifdef _WIN32
    _write(fd, line.c_str(), line.length());
#else
    write(fd, line.c_str(), line.length());
#endif
}

bool startsWithUppercase(const std::string &str) {
    if (str.empty()) {
        return false;
    }
    return std::isupper(static_cast<unsigned char>(str[0]));
}
