#ifndef UTILS_HPP
#define UTILS_HPP

#include "debug/colors.hpp" 
#include "logging/logger.hpp"

void print_message(int fd, const std::string& message, const std::string& color);
void print_server_config(const server_config& config);

#endif