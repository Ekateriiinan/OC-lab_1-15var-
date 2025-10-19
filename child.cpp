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
            writeString(STDOUT_FILENO, stringLine + "\n");  
        } else {
            writeString(STDERR_FILENO, INVALID_STRING);   
        }
        stringLine.clear();
    }
    return 0;
}
