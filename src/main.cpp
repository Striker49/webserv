#include "../include/webserv.hpp"

// Global exit flag for signal handling
volatile sig_atomic_t g_exit_flag = 0;

// Signal handler function
void signal_handler(int signal) {
    if (signal == SIGINT || signal == SIGQUIT) {
        g_exit_flag = 1;
    }
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);

    if (argc != 2) {
        log_instance.error("Usage: ./webserv <config_file>");
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file) {
        log_instance.error("Unable to open configuration file: " + std::string(argv[1]));
        return 1;
    }

    log_instance.debug("Configuration file opened successfully: " + std::string(argv[1]));

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    if (content.empty()) {
        log_instance.error("Configuration file is empty: " + std::string(argv[1]));
        return 1;
    }

    log_instance.debug("Configuration file read successfully");

    // Placeholder for tokenization and configuration parsing logic
    std::vector<token> tokens = tokenize(content);
    log_instance.debug("Configuration file tokenized successfully");

    std::vector<server_config> configs;
    if (!validate_tokens(tokens, configs)) {
        log_instance.error("Invalid configuration format in file: " + std::string(argv[1]));
        return 1;
    }

    log_instance.info("Configuration file validated successfully");

    // Print parsed configurations for testing
    print_server_configs(configs);

    // Setup and run the server sockets using SocketManager
    SocketManager socket_manager;
    for (size_t i = 0; i < configs.size(); ++i) {
        if (!socket_manager.add_socket(configs[i].get_listen())) {
            log_instance.error("Failed to set up server socket for config " + std::to_string(i + 1));
            return 1;
        }
    }

    log_instance.info("Starting server...");
    socket_manager.run();

    log_instance.info("Server shutting down");
    return 0;
}

// // Global exit flag for signal handling
// volatile sig_atomic_t g_exit_flag = 0;

// // Signal handler function
// void signal_handler(int signal) {
//     if (signal == SIGINT || signal == SIGQUIT) {
//         g_exit_flag = 1;
//     }
// }

// // Function to create and set up a socket
// int create_and_setup_socket(int port) {
//     int sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd < 0) {
//         log_instance.error("Failed to create socket");
//         return -1;
//     }
//     std::cout << COLOR_GREEN << "Socket created: " << sockfd << COLOR_RESET << std::endl;

//     sockaddr_in address;
//     std::memset(&address, 0, sizeof(address));
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(port);

//     if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
//         log_instance.error("Failed to bind socket");
//         close(sockfd);
//         return -1;
//     }
//     std::cout << COLOR_GREEN << "Socket bound: " << sockfd << " to port " << port << COLOR_RESET << std::endl;

//     if (listen(sockfd, 10) < 0) {
//         log_instance.error("Failed to listen on socket");
//         close(sockfd);
//         return -1;
//     }
//     std::cout << COLOR_GREEN << "Listening on socket: " << sockfd << COLOR_RESET << std::endl;

//     int flags = fcntl(sockfd, F_GETFL, 0);
//     if (flags == -1) {
//         log_instance.error("Failed to get socket flags");
//         close(sockfd);
//         return -1;
//     }

//     if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
//         log_instance.error("Failed to set socket to non-blocking");
//         close(sockfd);
//         return -1;
//     }
//     std::cout << COLOR_GREEN << "Socket set to non-blocking: " << sockfd << COLOR_RESET << std::endl;

//     return sockfd;
// }

// // Function to run the server
// void run_server(const std::vector<int>& listen_fds) {
//     fd_set master_set, read_set;
//     FD_ZERO(&master_set);
//     int max_fd = 0;

//     // Add listening sockets to master set
//     for (size_t i = 0; i < listen_fds.size(); ++i) {
//         int listen_fd = listen_fds[i];
//         FD_SET(listen_fd, &master_set);
//         if (listen_fd > max_fd) {
//             max_fd = listen_fd;
//         }
//     }

//     std::cout << COLOR_BLUE << "Entering server loop..." << COLOR_RESET << std::endl;
//     while (!g_exit_flag) {
//         read_set = master_set;
//         std::cout << "File descriptors before select: ";
//         for (int fd = 0; fd <= max_fd; ++fd) {
//             if (FD_ISSET(fd, &master_set)) {
//                 std::cout << fd << " ";
//             }
//         }
//         std::cout << std::endl;

//         int select_result = select(max_fd + 1, &read_set, NULL, NULL, NULL);
//         if (select_result < 0) {
//             if (errno == EINTR) {
//                 continue; // Interrupted by signal, continue loop
//             }
//             log_instance.error(std::string("Select error: ") + std::strerror(errno));
//             break;
//         }

//         std::cout << COLOR_BLUE << "Select returned, processing fds..." << COLOR_RESET << std::endl;
//         for (int fd = 0; fd <= max_fd; ++fd) {
//             if (FD_ISSET(fd, &read_set)) {
//                 // Check if it's a listening socket
//                 if (std::find(listen_fds.begin(), listen_fds.end(), fd) != listen_fds.end()) {
//                     sockaddr_in client_addr;
//                     socklen_t client_len = sizeof(client_addr);
//                     int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
//                     if (client_fd < 0) {
//                         log_instance.error("Failed to accept new connection");
//                         continue;
//                     }
//                     std::cout << COLOR_GREEN << "New connection accepted, client_fd: " << client_fd << COLOR_RESET << std::endl;
//                     FD_SET(client_fd, &master_set);
//                     if (client_fd > max_fd) {
//                         max_fd = client_fd;
//                     }
//                 } else {
//                     // Handle client message
//                     char buffer[1024];
//                     int bytes_received = recv(fd, buffer, sizeof(buffer), 0);

//                     if (bytes_received <= 0) {
//                         if (bytes_received == 0) {
//                             std::cout << COLOR_YELLOW << "Client disconnected, client_fd: " << fd << COLOR_RESET << std::endl;
//                         } else {
//                             log_instance.error("Recv error: " + std::string(std::strerror(errno)));
//                         }
//                         close(fd);
//                         FD_CLR(fd, &master_set);
//                     } else {
//                         buffer[bytes_received] = '\0';
//                         std::cout << COLOR_CYAN << "Received message from client_fd " << fd << ": " << buffer << COLOR_RESET << std::endl;
//                     }
//                 }
//             }
//         }
//     }

//     std::cout << COLOR_BLUE << "Exiting server loop..." << COLOR_RESET << std::endl;
//     for (int fd = 0; fd <= max_fd; ++fd) {
//         if (FD_ISSET(fd, &master_set)) {
//             close(fd);
//         }
//     }
// }

// int main(int argc, char *argv[]) {
//     signal(SIGINT, signal_handler);

//     if (argc != 2) {
//         log_instance.error("Usage: ./webserv <config_file>");
//         return 1;
//     }

//     std::ifstream file(argv[1]);
//     if (!file) {
//         log_instance.error("Unable to open configuration file: " + std::string(argv[1]));
//         return 1;
//     }

//     log_instance.debug("Configuration file opened successfully: " + std::string(argv[1]));

//     std::stringstream buffer;
//     buffer << file.rdbuf();
//     std::string content = buffer.str();

//     if (content.empty()) {
//         log_instance.error("Configuration file is empty: " + std::string(argv[1]));
//         return 1;
//     }

//     log_instance.debug("Configuration file read successfully");

//     // Placeholder for tokenization and configuration parsing logic
//     std::vector<token> tokens = tokenize(content);
//     log_instance.debug("Configuration file tokenized successfully");

//     std::vector<server_config> configs;
//     if (!validate_tokens(tokens, configs)) {
//         log_instance.error("Invalid configuration format in file: " + std::string(argv[1]));
//         return 1;
//     }

//     log_instance.info("Configuration file validated successfully");

//     // Print parsed configurations for testing
//     print_server_configs(configs);

//     // Setup and run the server sockets using SocketManager
//     std::vector<int> listen_fds;
//     for (size_t i = 0; i < configs.size(); ++i) {
//         int listen_fd = create_and_setup_socket(configs[i].get_listen());
//         if (listen_fd < 0) {
//             log_instance.error("Failed to set up server socket for config " + std::to_string(i + 1));
//             return 1;
//         }
//         listen_fds.push_back(listen_fd);
//     }

//     log_instance.info("Starting server...");
//     run_server(listen_fds);

//     log_instance.info("Server shutting down");

//     return 0;
// }

// // Global exit flag for signal handling
// volatile sig_atomic_t g_exit_flag = 0;

// // Signal handler function
// void signal_handler(int signal) {
//     if (signal == SIGINT || signal == SIGQUIT) {
//         g_exit_flag = 1;
//     }
// }

// // Function to create and set up a socket
// int create_and_setup_socket(int port) {
//     int sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd < 0) {
//         log_instance.error("Failed to create socket");
//         return -1;
//     }
//     std::cout << "Socket created: " << sockfd << std::endl;

//     sockaddr_in address;
//     std::memset(&address, 0, sizeof(address));
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(port);

//     if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
//         log_instance.error("Failed to bind socket");
//         close(sockfd);
//         return -1;
//     }
//     std::cout << "Socket bound: " << sockfd << " to port " << port << std::endl;

//     if (listen(sockfd, 10) < 0) {
//         log_instance.error("Failed to listen on socket");
//         close(sockfd);
//         return -1;
//     }
//     std::cout << "Listening on socket: " << sockfd << std::endl;

//     int flags = fcntl(sockfd, F_GETFL, 0);
//     if (flags == -1) {
//         log_instance.error("Failed to get socket flags");
//         close(sockfd);
//         return -1;
//     }

//     if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
//         log_instance.error("Failed to set socket to non-blocking");
//         close(sockfd);
//         return -1;
//     }
//     std::cout << "Socket set to non-blocking: " << sockfd << std::endl;

//     return sockfd;
// }

// // Function to run the server
// void run_server(const std::vector<int>& listen_fds) {
//     fd_set master_set, read_set;
//     FD_ZERO(&master_set);
//     int max_fd = 0;

//     // Add listening sockets to master set
//     for (size_t i = 0; i < listen_fds.size(); ++i) {
//         int listen_fd = listen_fds[i];
//         FD_SET(listen_fd, &master_set);
//         if (listen_fd > max_fd) {
//             max_fd = listen_fd;
//         }
//     }

//     std::cout << "Entering server loop..." << std::endl;
//     while (!g_exit_flag) {
//         read_set = master_set;
//         std::cout << "File descriptors before select: ";
//         for (int fd = 0; fd <= max_fd; ++fd) {
//             if (FD_ISSET(fd, &master_set)) {
//                 std::cout << fd << " ";
//             }
//         }
//         std::cout << std::endl;

//         int select_result = select(max_fd + 1, &read_set, NULL, NULL, NULL);
//         if (select_result < 0) {
//             if (errno == EINTR) {
//                 continue; // Interrupted by signal, continue loop
//             }
//             log_instance.error(std::string("Select error: ") + std::strerror(errno));
//             break;
//         }

//         std::cout << "Select returned, processing fds..." << std::endl;
//         for (int fd = 0; fd <= max_fd; ++fd) {
//             if (FD_ISSET(fd, &read_set)) {
//                 // Check if it's a listening socket
//                 if (std::find(listen_fds.begin(), listen_fds.end(), fd) != listen_fds.end()) {
//                     sockaddr_in client_addr;
//                     socklen_t client_len = sizeof(client_addr);
//                     int client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
//                     if (client_fd < 0) {
//                         log_instance.error("Failed to accept new connection");
//                         continue;
//                     }
//                     std::cout << "New connection accepted, client_fd: " << client_fd << std::endl;
//                     FD_SET(client_fd, &master_set);
//                     if (client_fd > max_fd) {
//                         max_fd = client_fd;
//                     }
//                 } else {
//                     // Handle client message
//                     char buffer[1024];
//                     int bytes_received = recv(fd, buffer, sizeof(buffer), 0);

//                     if (bytes_received <= 0) {
//                         if (bytes_received == 0) {
//                             std::cout << "Client disconnected, client_fd: " << fd << std::endl;
//                         } else {
//                             log_instance.error("Recv error: " + std::string(std::strerror(errno)));
//                         }
//                         close(fd);
//                         FD_CLR(fd, &master_set);
//                     } else {
//                         buffer[bytes_received] = '\0';
//                         std::cout << "Received message from client_fd " << fd << ": " << buffer << std::endl;
//                     }
//                 }
//             }
//         }
//     }

//     std::cout << "Exiting server loop..." << std::endl;
//     for (int fd = 0; fd <= max_fd; ++fd) {
//         if (FD_ISSET(fd, &master_set)) {
//             close(fd);
//         }
//     }
// }

// int main(int argc, char *argv[]) {
//     signal(SIGINT, signal_handler);

//     if (argc != 2) {
//         log_instance.error("Usage: ./webserv <config_file>");
//         return 1;
//     }

//     std::ifstream file(argv[1]);
//     if (!file) {
//         log_instance.error("Unable to open configuration file: " + std::string(argv[1]));
//         return 1;
//     }

//     log_instance.debug("Configuration file opened successfully: " + std::string(argv[1]));

//     std::stringstream buffer;
//     buffer << file.rdbuf();
//     std::string content = buffer.str();

//     if (content.empty()) {
//         log_instance.error("Configuration file is empty: " + std::string(argv[1]));
//         return 1;
//     }

//     log_instance.debug("Configuration file read successfully");

//     // Placeholder for tokenization and configuration parsing logic
//     std::vector<token> tokens = tokenize(content);
//     log_instance.debug("Configuration file tokenized successfully");

//     std::vector<server_config> configs;
//     if (!validate_tokens(tokens, configs)) {
//         log_instance.error("Invalid configuration format in file: " + std::string(argv[1]));
//         return 1;
//     }

//     log_instance.info("Configuration file validated successfully");

//     // Print parsed configurations for testing
//     print_server_configs(configs);

//     // Setup and run the server sockets using SocketManager
//     std::vector<int> listen_fds;
//     for (size_t i = 0; i < configs.size(); ++i) {
//         int listen_fd = create_and_setup_socket(configs[i].get_listen());
//         if (listen_fd < 0) {
//             log_instance.error("Failed to set up server socket for config " + std::to_string(i + 1));
//             return 1;
//         }
//         listen_fds.push_back(listen_fd);
//     }

//     log_instance.info("Starting server...");
//     run_server(listen_fds);

//     log_instance.info("Server shutting down");

//     return 0;
// }



// #include "../include/webserv.hpp"

// // Logger functions
// void log_info(const std::string &msg) {
//     std::cout << "[ INFO ] " << msg << std::endl;
// }

// void log_error(const std::string &msg) {
//     std::cerr << "[ ERROR ] " << msg << std::endl;
// }

// // Global exit flag for signal handling
// volatile sig_atomic_t g_exit_flag = 0;

// // Signal handler function
// void signal_handler(int signal) {
//     if (signal == SIGINT || signal == SIGQUIT) {
//         g_exit_flag = 1;
//     }
// }

// int create_and_setup_socket(int port) {
//     int sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd < 0) {
//         log_error("Failed to create socket");
//         return -1;
//     }
//     std::cout << "Socket created: " << sockfd << std::endl;

//     sockaddr_in address;
//     std::memset(&address, 0, sizeof(address));
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(port);

//     if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
//         log_error("Failed to bind socket");
//         close(sockfd);
//         return -1;
//     }
//     std::cout << "Socket bound: " << sockfd << " to port " << port << std::endl;

//     if (listen(sockfd, 10) < 0) {
//         log_error("Failed to listen on socket");
//         close(sockfd);
//         return -1;
//     }
//     std::cout << "Listening on socket: " << sockfd << std::endl;

//     int flags = fcntl(sockfd, F_GETFL, 0);
//     if (flags == -1) {
//         log_error("Failed to get socket flags");
//         close(sockfd);
//         return -1;
//     }

//     if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
//         log_error("Failed to set socket to non-blocking");
//         close(sockfd);
//         return -1;
//     }
//     std::cout << "Socket set to non-blocking: " << sockfd << std::endl;

//     return sockfd;
// }

// void run_server(int listen_fd) {
//     fd_set master_set, read_set;
//     FD_ZERO(&master_set);
//     FD_SET(listen_fd, &master_set);
//     int max_fd = listen_fd;

//     std::cout << "Entering server loop..." << std::endl;
//     while (!g_exit_flag) {
//         read_set = master_set;
//         std::cout << "File descriptors before select: ";
//         for (int fd = 0; fd <= max_fd; ++fd) {
//             if (FD_ISSET(fd, &master_set)) {
//                 std::cout << fd << " ";
//             }
//         }
//         std::cout << std::endl;

//         int select_result = select(max_fd + 1, &read_set, NULL, NULL, NULL);
//         if (select_result < 0) {
//             if (errno == EINTR) {
//                 continue; // Interrupted by signal, continue loop
//             }
//             log_error(std::string("Select error: ") + std::strerror(errno));
//             break;
//         }

//         std::cout << "Select returned, processing fds..." << std::endl;
//         if (FD_ISSET(listen_fd, &read_set)) {
//             sockaddr_in client_addr;
//             socklen_t client_len = sizeof(client_addr);
//             int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
//             if (client_fd < 0) {
//                 log_error("Failed to accept new connection");
//                 continue;
//             }
//             std::cout << "New connection accepted, client_fd: " << client_fd << std::endl;
//             FD_SET(client_fd, &master_set);
//             if (client_fd > max_fd) {
//                 max_fd = client_fd;
//             }
//         }
//     }

//     std::cout << "Exiting server loop..." << std::endl;
//     close(listen_fd);
//     for (int fd = 0; fd <= max_fd; ++fd) {
//         if (FD_ISSET(fd, &master_set)) {
//             close(fd);
//         }
//     }
// }

// int main(int argc, char *argv[]) {
//     signal(SIGINT, signal_handler);

//     int port = 8080; // Default port
//     if (argc == 2) {
//         port = std::atoi(argv[1]);
//     }

//     int listen_fd = create_and_setup_socket(port);
//     if (listen_fd < 0) {
//         return 1;
//     }

//     log_info("Starting server...");
//     run_server(listen_fd);
//     log_info("Server shutting down");

//     return 0;
// }


// #include "../include/webserv.hpp"

// // Logger instance
// logger log_instance("logs/server.log");

// // Global exit flag for signal handling
// volatile sig_atomic_t g_exit_flag = 0;

// // Signal handler function
// void signal_handler(int signal) {
//     if (signal == SIGINT || signal == SIGQUIT) {
//         g_exit_flag = 1;
//     }
// }

// int main(int argc, char *argv[]) {
//     signal(SIGINT, signal_handler);

//     if (argc != 2) {
//         log_instance.error("Usage: ./webserv <config_file>");
//         return 1;
//     }

//     std::ifstream file(argv[1]);
//     if (!file) {
//         log_instance.error("Unable to open configuration file: " + std::string(argv[1]));
//         return 1;
//     }

//     log_instance.debug("Configuration file opened successfully: " + std::string(argv[1]));

//     std::stringstream buffer;
//     buffer << file.rdbuf();
//     std::string content = buffer.str();

//     if (content.empty()) {
//         log_instance.error("Configuration file is empty: " + std::string(argv[1]));
//         return 1;
//     }

//     log_instance.debug("Configuration file read successfully");

//     std::vector<token> tokens = tokenize(content);
//     log_instance.debug("Configuration file tokenized successfully");

//     std::vector<server_config> configs;
//     if (!validate_tokens(tokens, configs)) {
//         log_instance.error("Invalid configuration format in file: " + std::string(argv[1]));
//         return 1;
//     }

//     log_instance.info("Configuration file validated successfully");

//     // Print parsed configurations for testing
//     print_server_configs(configs);

//     // Setup and run the server sockets using SocketManager
//     SocketManager socket_manager;
//     for (size_t i = 0; i < configs.size(); ++i) {
//         if (!socket_manager.add_socket(configs[i].get_listen())) {
//             log_instance.error("Failed to set up server socket for config " + std::to_string(i + 1));
//             return 1;
//         }
//     }

//     log_instance.info("Starting server...");
//     socket_manager.run();

//     log_instance.info("Server shutting down");

//     return 0;
// }
