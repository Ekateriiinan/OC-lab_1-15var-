#ifndef COMMON_H
#define COMMON_H

#include <string>

const std::string _USER_ALERT_VALID_OUT = "VALID: String starts with uppercase letter\n";
const std::string _USER_ALERT_INVALID_OUT = "INVALID: String does not start with uppercase letter\n";
const std::string _USER_ALERT_FILE_INPUT = "Enter filename for logging: ";
const std::string _USER_ALERT_STRING_INPUT = "Enter a string (Ctrl+D/Ctrl+Z to exit): ";
const std::string _USER_ALERT_ERROR_FILE = "Error opening file\n";

const int RD = 0;
const int WR = 1;

int readString(int fd, std::string &line);
void writeString(int fd, const std::string &line);
bool startsWithUppercase(const std::string &str);

#endif