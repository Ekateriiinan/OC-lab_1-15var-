#ifndef COMMON_H
#define COMMON_H

#include <string>

const std::string INVALID_STRING = "Error: string does not start with uppercase letter\n";
const std::string FILE_INPUT = "Enter filename: ";
const std::string STRING_INPUT = "Enter a string: ";
const std::string FILE_ERROR = "Error opening file\n";

const int RD = 0;
const int WR = 1;

int readString(int fd, std::string &line);
void writeString(int fd, const std::string &line);
bool startsWithUppercase(const std::string &str);

#endif
