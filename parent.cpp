#include <iostream>
#include <string>
#include <cstdlib>

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#include <fcntl.h>
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#define pipe(fds) _pipe(fds, 4096, _O_BINARY)
#define close _close
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#endif

#include "common.h"

int main() {
    int pipeFD1[2];
    int pipeFD2[2];

    if (pipe(pipeFD1) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipeFD2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    writeString(STDOUT_FILENO, _USER_ALERT_FILE_INPUT);
    
    std::string filename;
    if (readString(STDIN_FILENO, filename) == EOF) {
        writeString(STDOUT_FILENO, _USER_ALERT_ERROR_FILE);
        exit(EXIT_FAILURE);
    }

#ifdef _WIN32
    int fileFD = _open(filename.c_str(), _O_CREAT | _O_WRONLY | _O_APPEND, 0644);
#else
    int fileFD = open(filename.c_str(), O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
#endif

    if (fileFD < 0) {
        writeString(STDOUT_FILENO, _USER_ALERT_ERROR_FILE);
        perror("file");
        exit(EXIT_FAILURE);
    }

#ifdef _WIN32
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = (HANDLE)_get_osfhandle(pipeFD1[RD]);
    si.hStdOutput = (HANDLE)_get_osfhandle(pipeFD2[WR]);
    si.hStdError = (HANDLE)_get_osfhandle(fileFD);
    
    std::string command = "child.exe";
    if (!CreateProcess(NULL, (LPSTR)command.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
        exit(EXIT_FAILURE);
    }
    
    
    close(pipeFD1[RD]);
    close(pipeFD2[WR]);
    
    while (true) {
        writeString(STDOUT_FILENO, _USER_ALERT_STRING_INPUT);
        std::string str;
        if (readString(STDIN_FILENO, str) == EOF) {
            close(pipeFD1[WR]);
            break;
        }
        writeString(pipeFD1[WR], str + "\n");
        
        std::string response;
        readString(pipeFD2[RD], response);
        writeString(STDOUT_FILENO, response);
    }
    
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

#else
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipeFD2[RD]);
        close(pipeFD1[WR]);
        
        dup2(pipeFD2[WR], STDOUT_FILENO);
        dup2(pipeFD1[RD], STDIN_FILENO);
        dup2(fileFD, STDERR_FILENO);

        execl("./child", "./child", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        close(pipeFD1[RD]);
        close(pipeFD2[WR]);
        
        while (true) {
            writeString(STDOUT_FILENO, _USER_ALERT_STRING_INPUT);
            std::string str;
            if (readString(STDIN_FILENO, str) == EOF) {
                close(pipeFD1[WR]);
                break;
            }
            writeString(pipeFD1[WR], str + "\n");
            
            std::string response;
            readString(pipeFD2[RD], response);
            writeString(STDOUT_FILENO, response);
        }
        
        wait(NULL);
        close(pipeFD2[RD]);
    }
#endif

    close(fileFD);
    return 0;
}