#include "../../../include/webserv.hpp"

void print_message(int fd, const std::string& message, const std::string& color) {
    std::string output = color + message + COLOR_RESET + "\n";
    write(fd, output.c_str(), output.size());
}