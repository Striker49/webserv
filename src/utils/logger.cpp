#include "../../include/webserv.hpp"

logger log_instance("logs/server.log");

logger::logger(const std::string &log_file) : log_file(log_file) {
    log_stream.open(log_file, std::ios::app);
    if (!log_stream.is_open()) {
        std::cerr << "Failed to open log file: " << log_file << std::endl;
    }
}

logger::logger(const logger &other) : log_file(other.log_file) {
    if (log_stream.is_open()) {
        log_stream.close();
    }
    log_stream.open(log_file, std::ios::app);
}

logger &logger::operator=(const logger &other) {
    if (this != &other) {
        log_file = other.log_file;
        if (log_stream.is_open()) {
            log_stream.close();
        }
        log_stream.open(log_file, std::ios::app);
    }
    return *this;
}

logger::~logger() {
    if (log_stream.is_open()) {
        log_stream.close();
    }
}

void logger::log(log_level level, const std::string &message) {
    if (log_stream.is_open()) {
        log_stream << get_level_string(level) << message << std::endl;
    }
}

void logger::debug(const std::string &message) {
    log(LOG_DEBUG, message);
}

void logger::info(const std::string &message) {
    log(LOG_INFO, message);
}

void logger::warning(const std::string &message) {
    log(LOG_WARNING, message);
}

void logger::error(const std::string &message) {
    log(LOG_ERROR, message);
}

std::string logger::get_color(log_level level) {
    switch (level) {
        case LOG_DEBUG: return COLOR_BLUE;
        case LOG_INFO: return COLOR_GREEN;
        case LOG_WARNING: return COLOR_YELLOW;
        case LOG_ERROR: return COLOR_RED;
        default: return COLOR_RESET;
    }
}

std::string logger::get_level_string(log_level level) {
    switch (level) {
        case LOG_DEBUG: return "[DEBUG] ";
        case LOG_INFO: return "[INFO] ";
        case LOG_WARNING: return "[WARNING] ";
        case LOG_ERROR: return "[ERROR] ";
        default: return "[UNKNOWN] ";
    }
}
