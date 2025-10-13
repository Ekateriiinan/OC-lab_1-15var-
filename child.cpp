#include <string>

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#else
#include <unistd.h>
#endif

#include "common.h"

int main() {
    std::string stringLine;
    
    while (readString(STDIN_FILENO, stringLine) != EOF) {
        if (startsWithUppercase(stringLine)) {
            writeString(STDERR_FILENO, stringLine + "\n");
            writeString(STDOUT_FILENO, _USER_ALERT_VALID_OUT);
        } else {
            writeString(STDOUT_FILENO, _USER_ALERT_INVALID_OUT);
        }
        stringLine.clear();
    }
    return 0;
}