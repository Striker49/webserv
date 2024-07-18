#include "../../include/webserv.hpp"

std::string trim_connection_header(const std::string &connection_header) {
    return trim_whitespace(connection_header);
}

bool is_empty_header(const std::string &trimmed_header) {
    return trimmed_header.empty();
}

bool is_close_header(const std::string &trimmed_header) {
    return strcmp(trimmed_header.c_str(), "close") == 0;
}

bool is_keep_alive_header(const std::string &trimmed_header) {
    return strcmp(trimmed_header.c_str(), "keep-alive") == 0;
}

bool is_valid_http_method(const std::string &method, const std::vector<std::string>& allowed_methods) {
    http_methods methods;
    return methods.is_valid_method(method, allowed_methods);
}

bool is_valid_http_version(const std::string &http_version) {
    return http_version == "HTTP/1.1";
}

bool is_complete_request_line(const std::string &method, const std::string &uri, const std::string &http_version) {
    return !method.empty() && !uri.empty() && !http_version.empty();
}

bool validate_empty_header(const std::string &trimmed_header) {
    if (is_empty_header(trimmed_header)) {
        log_instance.debug("Connection header is empty or not present.");
        return true;
    }
    return false;
}

bool validate_close_header(const std::string &trimmed_header) {
    if (is_close_header(trimmed_header)) {
        log_instance.debug("Connection header is 'close'.");
        return true;
    }
    return false;
}

bool validate_keep_alive_header(const std::string &trimmed_header) {
    if (is_keep_alive_header(trimmed_header)) {
        log_instance.debug("Connection header is 'keep-alive'.");
        return true;
    }
    return false;
}

bool log_invalid_header(const std::string &trimmed_header) {
    log_instance.debug("Connection header is invalid: " + trimmed_header);
    return false;
}

bool is_valid_connection_header(const std::string &connection_header) {
    std::string trimmed_header = trim_connection_header(connection_header);
    return validate_empty_header(trimmed_header) ||
           validate_close_header(trimmed_header) ||
           validate_keep_alive_header(trimmed_header) ||
           log_invalid_header(trimmed_header);
}

bool validate_method(const std::string &method, const std::vector<std::string>& allowed_methods) {
    return is_valid_http_method(method, allowed_methods);
}

bool validate_http_version(const std::string &http_version) {
    return is_valid_http_version(http_version);
}

bool validate_request_line(const std::string &method, const std::string &uri, const std::string &http_version) {
    return is_complete_request_line(method, uri, http_version);
}

bool validate_connection_header(const std::string &connection_header) {
    return is_valid_connection_header(connection_header);
}

int is_valid_request(const http_request &request, const std::vector<std::string>& allowed_methods) {
    http_methods methods;
    if (!validate_connection_header(request.get_header_value("Connection")))
        return 400;
    if (!validate_http_version(request.get_http_version()))
        return 505;
    if (!validate_request_line(request.get_method(), request.get_uri(), request.get_http_version()))
        return 405;
    return 0;
}

std::string build_response_string(const http_response& response) {
    return response.build_response();
}

bool send_data(int client_sock, const std::string& data) {
    ssize_t total_sent = 0;
    size_t data_length = data.size();

    while (total_sent < data_length) {
        ssize_t sent = write(client_sock, data.c_str() + total_sent, data_length - total_sent);
        if (sent < 0) {
            log_instance.error("Failed to send data, error: " + std::string(strerror(errno)));
            return false;
        }
        total_sent += sent;
    }
    return true;
}

void close_connection(int client_sock, bool keep_alive) {
    if (!keep_alive) {
        close(client_sock);
        log_instance.info("Connection closed for client socket fd " + std::to_string(client_sock));
    }
}

void send_response(int client_sock, const http_response& response, bool keep_alive) {
    std::string response_str = build_response_string(response);
    // std::cout << "response_str: " << response_str << std::endl;
    log_response(response_str);
    bool success = send_data(client_sock, response_str);
    ssize_t log_size = -1;
    if (success) {
        log_size = response_str.size();
    }
    log_response_status(client_sock, log_size, response_str.size());
    close_connection(client_sock, keep_alive);
}

bool parse_and_validate_request(const std::string &total_request, http_request &request, http_response &response, const std::vector<std::string>& allowed_methods) {
    if (!request.parse_request(total_request)) {
        response.set_status_code(400);
        response.set_body("Bad Request.");
        response.add_header("Content-Type", "text/plain");
        response.add_header("Content-Length", std::to_string(response.get_body().size()));
        return false;
    }
    int status_code = is_valid_request(request, allowed_methods);
    if (status_code != 0) {
        std::cout << "status_code " << status_code << std::endl;
        std::cout << "response.status_code_to_message(status_code " << response.status_code_to_message(status_code) << std::endl;
        response.set_status_code(status_code); // Method Not Allowed
        response.set_body(response.status_code_to_message(status_code));
        response.add_header("Content-Type", "text/plain");
        response.add_header("Content-Length", std::to_string(response.get_body().size()));
        return false;
    }
    return true;
}

bool set_connection_header(const http_request &request, bool &keep_alive) {
    std::string connection_header = request.get_header_value("Connection");
    log_instance.debug("Connection header from request: " + connection_header);
    if (!is_valid_connection_header(connection_header)) {
        log_instance.debug("Invalid connection header, setting response to 400.");
        return false;
    }
    keep_alive = (connection_header == "keep-alive");
    return true;
}

bool file_exists(const std::string& file_path) {
    print_message(STDOUT_FILENO, "Checking if file exists: " + file_path, COLOR_YELLOW);
    std::string short_path = file_path;
    // if (short_path.find("www") != 0)
    //     short_path.erase(0, short_path.find("www"));
    std::ifstream file(short_path);
    bool exists = file.good();
    if (exists) {
        print_message(STDOUT_FILENO, "File exists.", COLOR_YELLOW);
    } else {
        print_message(STDOUT_FILENO, "File does not exist.", COLOR_YELLOW);
    }
    return exists;
}

std::string read_file_content(const std::string& file_path) {
    print_message(STDOUT_FILENO, "Reading file content from: " + file_path, COLOR_YELLOW);
    std::ifstream file(file_path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string get_error_page_path(const server_config& server_conf, int status_code) {
    print_message(STDOUT_FILENO, "Getting error page path for status code: " + std::to_string(status_code), COLOR_RED);
    std::map<int, std::string> error_pages = server_conf.get_error_pages();
    if (error_pages.find(status_code) != error_pages.end()) {
        std::string path = error_pages[status_code];
        print_message(STDOUT_FILENO, "Error page path found: " + path, COLOR_RED);
        return path;
    }
    print_message(STDOUT_FILENO, "No error page path found for status code: " + std::to_string(status_code), COLOR_RED);
    return "";
}

void serve_error_page(http_response &response, const std::string &error_page_path, int status_code, const std::string &default_body) {
    print_message(STDOUT_FILENO, "Serving error page for status code: " + std::to_string(status_code), COLOR_RED);
    if (file_exists(error_page_path)) {
        response.set_status_code(status_code);
        response.set_body(read_file_content(error_page_path));
        response.add_header("Content-Type", "text/html");
    } else {
        response.set_status_code(status_code);
        response.set_body(default_body);
        response.add_header("Content-Type", "text/plain");
    }
    response.add_header("Content-Length", std::to_string(response.get_body().size()));
}

std::string get_file_path_from_uri(http_request &request, const std::string &uri, const std::vector<location_config>& locations, const std::vector<server_config>& configs) {
    print_message(STDOUT_FILENO, "Getting file path from URI: " + uri, COLOR_CYAN);
    std::string path_with_www = "www" + uri;
    std::string path = uri;
    if (path.find("www") != std::string::npos)
        path.erase(0, 3);
    if (is_cgi(configs, request))
    {
        if (request.get_query_params().empty())
            return (uri);
        path = "/cgi";
    }
    for (std::vector<location_config>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
        if (path.compare(it->get_path()) == 0) 
        {
            // path = it->get_root();
            request.set_root(it->get_root());
            request.set_index(it->get_index());
            request.set_autoindex(it->get_autoindex());
            print_message(STDOUT_FILENO, "File path found: " + uri, COLOR_CYAN);
            if (path == "/cgi")
                return (request.get_root() + uri);
            else
                return ("www" + path);
        }
    }
    request.set_root("www" + path);
    print_message(STDOUT_FILENO, "No matching file path found for URI: " + uri, COLOR_CYAN);
    return ("www" + path);
}

void process_request(int client_sock, const std::string &total_request, bool &keep_alive, const std::vector<server_config>& configs, int config_index, std::string binary) {
    print_message(STDOUT_FILENO, "Processing request...", COLOR_MAGENTA);
    http_request request;
    http_response response;

    request.set_bin_body(binary);
    print_message(STDOUT_FILENO, "Parsing and validating request...", COLOR_MAGENTA);
    if (!parse_and_validate_request(total_request, request, response, configs[config_index].get_default_methods())) {
        send_response(client_sock, response, keep_alive);
        return;
    }

    print_message(STDOUT_FILENO, "Request URI: " + request.get_uri(), COLOR_CYAN);

    print_message(STDOUT_FILENO, "Setting connection header...", COLOR_MAGENTA);
    if (!set_connection_header(request, keep_alive)) {
        std::string error_page_path = get_error_page_path(configs[config_index], 400);
        serve_error_page(response, error_page_path, 400, "Bad Request.");
        send_response(client_sock, response, keep_alive);
        return;
    }
    method_router(request.get_method(), request, response, configs, config_index);
    send_response(client_sock, response, keep_alive);
    log_request(client_sock, total_request);
}

void set_select_timeout(struct timeval &timeout, int seconds) {
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;
}

void initialize_fd_set(fd_set &read_fds, int client_sock) {
    FD_ZERO(&read_fds);
    FD_SET(client_sock, &read_fds);
}

bool handle_select_error(int activity) {
    if (activity < 0) {
        log_instance.error("Select error: " + std::string(strerror(errno)));
        return false;
    }
    return true;
}

int read_client_data(int client_sock, std::string &total_request, fd_set &master_set_read, fd_set &master_set_write, std::vector<char> *data) {
    ssize_t buffer_size = 4096;
    char buffer[buffer_size + 1];
    int len = buffer_size;
    ssize_t bytes_read;
    struct timeval timeout = {5, 0};
    fd_set read_fds;

    FD_ZERO(&read_fds);
    FD_SET(client_sock, &read_fds);

    int activity = select(client_sock + 1, &read_fds, NULL, NULL, &timeout);
    if (activity < 0) {
        log_instance.error("Select error: " + std::string(strerror(errno)));
        return false;
    }
    if (activity == 0) {
        log_instance.debug("Timeout occurred while reading from client socket fd: " + std::to_string(client_sock));
        return false;
    }
if (FD_ISSET(client_sock, &read_fds)) {
        bytes_read = read(client_sock, buffer, sizeof(buffer) - 1);
        if (bytes_read == buffer_size) {
            // buffer[bytes_read] = '\0';
            data->insert(data->end(), buffer, buffer + bytes_read);
            total_request.append(buffer);
            log_instance.debug("Data read from client socket fd " + std::to_string(client_sock) + ": " + std::string(buffer));
            return 2;
        }
        if (bytes_read < buffer_size) {
            // buffer[bytes_read] = '\0';
            data->insert(data->end(), buffer, buffer + bytes_read);
            total_request.append(buffer);
            return 1;
        }
        if (bytes_read <= 0) {
            close_connection(client_sock, &master_set_read, &master_set_write);
            return false;
        }
    }
    return false;
}

void init_request(int client_sock, std::string &total_request, bool &keep_alive, const std::vector<server_config>& configs, int config_index, fd_set &master_set_read, fd_set &master_set_write, std::vector<char> *data) {
    int is_complete = read_client_data(client_sock, total_request, master_set_read, master_set_write, data);
    http_methods methods;

    if (is_complete > 1)
        return;
    else if (is_complete) {
        std::string bin_request(data->begin(), data->end());
        std::cout << "bin_request: " << bin_request << std::endl;
        log_instance.debug("Complete request received: " + total_request);
        process_request(client_sock, total_request, keep_alive, configs, config_index, bin_request);
        total_request.clear();
    } else {
        log_instance.debug("Incomplete request or no data received.");
        keep_alive = false;
    }
}

void close_connection(int client_sock, fd_set* master_set_read, fd_set* master_set_write) {
    close(client_sock);
    FD_CLR(client_sock, master_set_read);
    FD_CLR(client_sock, master_set_write);
    log_instance.info("Connection closed for client socket fd " + std::to_string(client_sock));
}

void handle_client(int client_sock, fd_set* master_set_read, fd_set* master_set_write, const std::vector<server_config>& configs, int config_index) {
    std::string total_request;
    std::vector<char> data;
    bool keep_alive = true;

    log_new_connection(client_sock);
    while (keep_alive) {
        init_request(client_sock, total_request, keep_alive, configs, config_index, *master_set_read, *master_set_write, &data);
    }
    close_connection(client_sock, master_set_read, master_set_write);
}

void set_default_config_path(std::string& config_path) {
    config_path = "configs/default.conf";
}

bool validate_argument_count(int argc) {
    if (argc > 2) {
        log_instance.error("Usage: ./webserv <config_file>");
        print_message(STDERR_FILENO, "Usage: ./webserv <config_file>", COLOR_RED);
        return false;
    }
    return true;
}

bool validate_config_file_extension(const std::string& config_path) {
    if (config_path.size() < 5 || config_path.substr(config_path.size() - 5) != ".conf") {
        log_instance.error("Configuration file must have a .conf extension");
        print_message(STDERR_FILENO, "Error: Configuration file must have a .conf extension", COLOR_RED);
        return false;
    }
    return true;
}

bool initialize_configuration(int argc, char* argv[], std::string& config_path) {
    set_default_config_path(config_path);
    if (!validate_argument_count(argc)) {
        return false;
    }
    if (argc == 2) {
        config_path = argv[1];
        if (!validate_config_file_extension(config_path)) {
            return false;
        }
    }
    return true;
}

bool open_config_file(const std::string& config_path, std::ifstream& config_file) {
    config_file.open(config_path);
    if (!config_file) {
        log_instance.error("Unable to open configuration file: " + config_path);
        print_message(STDERR_FILENO, "Error: Unable to open configuration file: " + config_path, COLOR_RED);
        return false;
    }
    return true;
}

std::string read_config_file(std::ifstream& config_file) {
    std::stringstream buffer;
    buffer << config_file.rdbuf();
    return buffer.str();
}

bool validate_and_tokenize_config(const std::string& content, std::vector<server_config>& configs) {
    std::vector<token> tokens = tokenize(content);
    if (!validate_tokens(tokens, configs)) {
        log_instance.error("Invalid configuration format.");
        print_message(STDERR_FILENO, "", COLOR_RED);
        return false;
    }
    return true;
}

bool load_server_configurations(const std::string& config_path, std::vector<server_config>& configs) {
    std::ifstream config_file;
    if (!open_config_file(config_path, config_file)) {
        return false;
    }
    std::string content = read_config_file(config_file);
    if (!validate_and_tokenize_config(content, configs)) {
        return false;
    }
    log_configuration_success();
    return true;
}

bool configure_socket(Socket* server, const server_config& config, int port) {
    return server->create() &&
           server->set_reuse_address() &&
           server->bind(port) &&
           server->listen(SOMAXCONN) &&
           server->set_non_blocking();
}

void add_socket_to_master_set(Socket* server, fd_set& master_set, int& max_fd) {
    FD_SET(server->get_socket_fd(), &master_set);
    if (server->get_socket_fd() > max_fd) {
        max_fd = server->get_socket_fd();
    }
}

void log_server_setup(const server_config& config, int port) {
    log_instance.info("Server set up on port: " + std::to_string(port));
}

bool setup_servers(const std::vector<server_config>& configs, std::vector<Socket*>& servers, fd_set& master_set_read, int& max_fd) {
    for (size_t i = 0; i < configs.size(); ++i) {
        const std::vector<int>& ports = configs[i].get_listen();
        for (std::vector<int>::const_iterator it = ports.begin(); it != ports.end(); ++it) {
            int port = *it;
            Socket* server = new Socket();
            server->set_config_index(i);  // Set the config index
            if (!configure_socket(server, configs[i], port)) {
                log_instance.error("Failed to set up server on port: " + std::to_string(port));
                delete server;
                return false;
            }
            servers.push_back(server);
            add_socket_to_master_set(server, master_set_read, max_fd);
            log_server_setup(configs[i], port);
        }
        print_server_config(configs[i]);
    }
    return true;
}

int wait_for_activity(fd_set& master_set_read, fd_set& master_set_write, int max_fd) {
    fd_set read_fds = master_set_read;
    fd_set write_fds = master_set_write;
    struct timeval timeout = {5, 0};
    log_instance.debug("Waiting for activity...");
    int activity = select(max_fd + 1, &read_fds, &write_fds, NULL, &timeout);
    // if (activity < 0) {
    //     log_instance.error("Select error occurred.");
    // }
    return activity;
}

void process_server_sockets(std::vector<Socket*>& servers, fd_set& master_set_read, int& max_fd, std::vector<int>& client_sockets, fd_set& read_fds, std::map<int, int>& client_to_config_map) {
    for (size_t i = 0; i < servers.size(); ++i) {
        if (FD_ISSET(servers[i]->get_socket_fd(), &read_fds)) {
            log_instance.debug("Incoming connection detected on server socket.");
            int client_socket = servers[i]->accept();
            if (client_socket != -1) {
                client_to_config_map[client_socket] = servers[i]->get_config_index(); // Store the config index for the client
                FD_SET(client_socket, &master_set_read);
                if (client_socket > max_fd) {
                    max_fd = client_socket;
                }
                client_sockets.push_back(client_socket);
            }
        }
    }
}

void process_client_sockets(std::vector<int>& client_sockets, std::map<int, std::string>& response_buffers, fd_set& master_set_read, fd_set& master_set_write, fd_set& read_fds, fd_set& write_fds, const std::vector<server_config>& configs, const std::vector<Socket*>& servers, const std::map<int, int>& client_to_config_map) {
    for (std::vector<int>::iterator it = client_sockets.begin(); it != client_sockets.end();) {
        int client_socket = *it;

        if (FD_ISSET(client_socket, &read_fds)) {
            int config_index = client_to_config_map.at(client_socket); // Get the config index for the client

            handle_client(client_socket, &master_set_read, &master_set_write, configs, config_index);
            it = client_sockets.erase(it);
        } else if (FD_ISSET(client_socket, &write_fds)) {
            if (!response_buffers[client_socket].empty()) {
                ssize_t bytes_sent = send(client_socket, response_buffers[client_socket].data(), response_buffers[client_socket].size(), 0);
                if (bytes_sent <= 0) {
                    log_instance.error("Error writing to client socket: " + std::string(strerror(errno)));
                    close_connection(client_socket, &master_set_read, &master_set_write);
                    it = client_sockets.erase(it);
                } else {
                    response_buffers[client_socket].erase(0, bytes_sent);
                    if (response_buffers[client_socket].empty()) {
                        FD_CLR(client_socket, &master_set_write);
                    }
                    ++it;
                }
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
}

void run_server_loop(std::vector<Socket*>& servers, fd_set& master_set_read, fd_set& master_set_write, int& max_fd, const std::vector<server_config>& configs) {
    std::vector<int> client_sockets;
    std::map<int, std::string> response_buffers;
    std::map<int, int> client_to_config_map; // Map to store client socket to config index

    bool running = true;

    while (running && !g_exit_flag) {
        int activity = wait_for_activity(master_set_read, master_set_write, max_fd);
        if (activity <= 0) {
            if (activity < 0) {
                log_instance.error("Select error occurred.");
                running = false;
            }
            continue;
        }
        log_instance.debug("Activity detected.");
        fd_set read_fds = master_set_read;
        fd_set write_fds = master_set_write;
        process_server_sockets(servers, master_set_read, max_fd, client_sockets, read_fds, client_to_config_map);
        process_client_sockets(client_sockets, response_buffers, master_set_read, master_set_write, read_fds, write_fds, configs, servers, client_to_config_map);
    }
}

void cleanup_servers(std::vector<Socket*>& servers) {
    for (size_t i = 0; i < servers.size(); ++i) {
        servers[i]->close_socket();
        delete servers[i];
    }
}
