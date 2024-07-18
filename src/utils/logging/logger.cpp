#include "../../../include/webserv.hpp"

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

void log_and_print_shutdown_message() {
    log_instance.info("Shutdown signal received, stopping servers...");
    print_message(STDERR_FILENO, "Shutdown signal received, stopping servers...", COLOR_MAGENTA);
}

void log_configuration_success() {
    log_instance.info("Configuration file validated successfully");
    print_message(STDERR_FILENO, "Configuration file validated successfully", COLOR_MAGENTA);
}

void log_new_connection(int client_sock) {
    log_instance.debug("New client connected, socket fd: " + std::to_string(client_sock));
}

bool log_and_return(const std::string &message, bool return_value) {
    log_instance.debug(message);
    return return_value;
}

void log_request(int client_sock, const std::string &total_request) {
    log_instance.debug("Received complete request from client socket fd " + std::to_string(client_sock) + ": " + total_request);
    print_message(STDOUT_FILENO, "Received from client: " + total_request, COLOR_GREEN);
}

void log_response_status(int client_sock, ssize_t total_sent, size_t response_length) {
    if (total_sent == response_length) {
        log_instance.info("Response completely sent to client socket fd " + std::to_string(client_sock) + ". Total bytes sent: " + std::to_string(total_sent));
    } else {
        log_instance.warning("Incomplete response sent to client socket fd " + std::to_string(client_sock));
    }
}

void log_response(const std::string& response_str) {
    std::istringstream stream(response_str);
    std::string line;
    while (std::getline(stream, line)) {
        log_instance.debug("Response: " + line);
        if (line.empty()) return;
    }
}
