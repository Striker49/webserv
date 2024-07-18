#include "../../../include/webserv.hpp"

void print_server_config(const server_config& config) {
    const int propertyWidth = 16; 
    const int valueWidth = 30; 
    const int totalWidth = propertyWidth + valueWidth + 3; 
    std::cout << COLOR_BLUE << std::string(totalWidth, '=') << COLOR_RESET << std::endl;
    std::cout << COLOR_BLUE << std::left
              << std::setw(propertyWidth) << "| Property" << "|"
              << std::setw(valueWidth) << " Value" << "|"
              << COLOR_RESET << std::endl;
    std::cout << COLOR_BLUE << std::string(totalWidth, '_') << COLOR_RESET << std::endl; // Header underline

    // Server properties
    std::cout << COLOR_GREEN << std::setw(propertyWidth) << "| Listen Ports" << "|";
    const std::vector<int>& ports = config.get_listen();
    std::ostringstream ports_stream;
    for (size_t i = 0; i < ports.size(); ++i) {
        if (i > 0) ports_stream << ", ";
        ports_stream << ports[i];
    }
    std::cout << std::setw(valueWidth) << ports_stream.str() << "|" << std::endl;

    std::cout << COLOR_GREEN << std::setw(propertyWidth) << "| Server Name" << "|"
              << std::setw(valueWidth) << config.get_server_name() << "|"
              << COLOR_RESET << std::endl;
    std::cout << COLOR_GREEN << std::setw(propertyWidth) << "| Root Directory" << "|"
              << std::setw(valueWidth) << config.get_root() << "|"
              << COLOR_RESET << std::endl;
    std::cout << COLOR_GREEN << std::setw(propertyWidth) << "| Max Body Size" << "|"
              << std::setw(valueWidth) << config.get_client_max_body_size() << "|"
              << COLOR_RESET << std::endl;

    // Locations header
    std::cout << COLOR_YELLOW << std::string(totalWidth, '=') << COLOR_RESET << std::endl; 
    std::cout << COLOR_YELLOW << "| Locations:" << COLOR_RESET << std::endl;
    std::cout << COLOR_YELLOW << std::string(totalWidth, '-') << COLOR_RESET << std::endl;

    // Locations details
    const std::vector<location_config>& locations = config.get_locations();
    for (std::vector<location_config>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        std::cout << COLOR_CYAN << std::setw(propertyWidth) << "| Path" << "|"
                  << std::setw(valueWidth) << it->get_path() << "|"
                  << COLOR_RESET << std::endl;
        std::cout << COLOR_CYAN << std::setw(propertyWidth) << "| Root" << "|"
                  << std::setw(valueWidth) << it->get_root() << "|"
                  << COLOR_RESET << std::endl;
        std::cout << COLOR_CYAN << std::setw(propertyWidth) << "| Index" << "|"
                  << std::setw(valueWidth) << it->get_index() << "|"
                  << COLOR_RESET << std::endl;
        std::cout << COLOR_CYAN << std::setw(propertyWidth) << "| Autoindex" << "|"
                  << std::setw(valueWidth) << (it->get_autoindex() ? "true" : "false") << "|"
                  << COLOR_RESET << std::endl;
        std::cout << COLOR_CYAN << std::string(totalWidth, '-') << COLOR_RESET << std::endl;
    }
}
