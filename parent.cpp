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
    int pipe1[2];
    int pipe2[2];

    if (pipe(pipe1) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    writeString(STDOUT_FILENO, FILE_INPUT);
    
    std::string filename;
    if (readString(STDIN_FILENO, filename) == EOF) {
        writeString(STDOUT_FILENO, FILE_ERROR);
        exit(EXIT_FAILURE);
    }

    #ifdef _WIN32
        int file = _open(filename.c_str(), _O_CREAT | _O_WRONLY | _O_APPEND, 0644);
    #else
        int file = open(filename.c_str(), O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
    #endif

        if (file < 0) {
            writeString(STDOUT_FILENO, FILE_ERROR);
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
    si.hStdInput = (HANDLE)_get_osfhandle(pipe1[RD]);
    si.hStdOutput = (HANDLE)_get_osfhandle(pipe2[WR]);  
    si.hStdError = (HANDLE)_get_osfhandle(pipe2[WR]);   
    
    std::string command = "child.exe";
    if (!CreateProcess(NULL, (LPSTR)command.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
        exit(EXIT_FAILURE);
    }
    
    close(pipe1[RD]);
    close(pipe2[WR]);
    
    while (true) {
        writeString(STDOUT_FILENO, STRING_INPUT);
       
        std::string str;
        if (readString(STDIN_FILENO, str) == EOF) {
            close(pipe1[WR]);
            break;
        }
        writeString(pipe1[WR], str + "\n");

        std::string response;
        readString(pipe2[RD], response); 
        writeString(file, response);
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
            close(pipe2[RD]);
            close(pipe1[WR]);

            dup2(pipe1[RD], STDIN_FILENO);
            dup2(pipe2[WR], STDOUT_FILENO); 
            dup2(pipe2[WR], STDERR_FILENO);  

            execl("./child", "./child", NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        } else {
            close(pipe1[RD]);
            close(pipe2[WR]);
            
            while (true) {
                writeString(STDOUT_FILENO, STRING_INPUT);
                std::string str;
                if (readString(STDIN_FILENO, str) == EOF) {
                    close(pipe1[WR]);
                    break;
                }
                writeString(pipe1[WR], str + "\n");
                
                std::string response;
                readString(pipe2[RD], response);
                writeString(file, response);
            }
            
            wait(NULL);
            close(pipe2[RD]);
        }
    #endif

    close(file);
    return 0;
}
