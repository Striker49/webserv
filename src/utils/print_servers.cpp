#include <iomanip> // For std::setw
#include <sstream> // For std::ostringstream
#include "../../include/webserv.hpp"

void print_server_configs(const std::vector<server_config>& configs) {
    std::cout << std::endl;
    std::cout << std::setw(15) << std::left << "Server Block"
              << std::setw(10) << "Listen"
              << std::setw(15) << "Server Name"
              << std::setw(25) << "Root"
              << std::setw(20) << "Max Body Size"
              << std::setw(20) << "Upload Path"
              << std::setw(20) << "Allowed Methods"
              << std::endl;
    std::cout << std::string(130, '-') << std::endl;

    for (size_t i = 0; i < configs.size(); ++i) {
        std::ostringstream ss;
        ss << "Server " << (i + 1);

        std::cout << std::setw(15) << std::left << ss.str()
                  << std::setw(10) << configs[i].get_listen()
                  << std::setw(15) << configs[i].get_server_name()
                  << std::setw(25) << configs[i].get_root()
                  << std::setw(20) << configs[i].get_client_max_body_size()
                  << std::setw(20) << configs[i].get_upload_path();

        // Print allowed methods
        std::string allowed_methods;
        const std::vector<std::string>& methods = configs[i].get_allowed_methods();
        for (std::vector<std::string>::const_iterator it = methods.begin(); it != methods.end(); ++it) {
            allowed_methods += *it + " ";
        }
        std::cout << std::setw(20) << allowed_methods;

        std::cout << std::endl;

        // Print location configs
        const std::vector<location_config>& locations = configs[i].get_locations();
        for (std::vector<location_config>::const_iterator loc_it = locations.begin(); loc_it != locations.end(); ++loc_it) {
            std::cout << std::setw(15) << " "
                      << std::setw(10) << " "
                      << std::setw(15) << "Location"
                      << std::setw(25) << loc_it->get_path()
                      << std::setw(20) << loc_it->get_root()
                      << std::setw(30) << loc_it->get_index()
                      << std::setw(20) << (loc_it->get_autoindex() ? "true" : "false");

            // Print CGI configurations
            std::string cgi_config;
            const std::map<std::string, std::string>& cgi_map = loc_it->get_cgi();
            for (std::map<std::string, std::string>::const_iterator cgi_it = cgi_map.begin(); cgi_it != cgi_map.end(); ++cgi_it) {
                std::ostringstream cgi_ss;
                cgi_ss << cgi_it->first << ":" << cgi_it->second << " ";
                cgi_config += cgi_ss.str();
            }
            std::cout << std::setw(30) << cgi_config;

            std::cout << std::endl;
            std::cout << std::endl;
        }
    }
}
