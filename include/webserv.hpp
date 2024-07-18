#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "server/server.hpp"
#include "cgi/cgi.hpp"
#include "sockets/sockets.hpp"
#include "http/http.hpp"
#include "http/methods/http_methods.hpp"
#include "http/request/http_request.hpp"
#include "utils/utils.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip> 
#include <csignal>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

void close_connection(int client_sock, bool keep_alive);
void send_response(int client_sock, const http_response& response, bool keep_alive);
bool parse_and_validate_request(const std::string &total_request, http_request &request, http_response &response);
void set_response(const http_request &request, http_response &response);
bool set_connection_header(const http_request &request, bool &keep_alive);
bool file_exists(const std::string& file_path);
std::string read_file_content(const std::string& file_path);
const location_config* find_location_config(const std::vector<location_config>& locations, const std::string& uri);
std::string get_error_page_path(const server_config& server_conf, int status_code);
void serve_error_page(http_response &response, const std::string &error_page_path, int status_code, const std::string &default_body);
std::string get_file_path_from_uri(http_request &request, const std::string &uri, const std::vector<location_config>& locations, const std::vector<server_config>& configs);
void process_request(int client_sock, const std::string &total_request, bool &keep_alive, const std::vector<server_config>& configs, int config_index, std::string binary);
bool process_post_request(http_request &request, http_response &response, const std::vector<server_config>& configs, int config_index);
void set_select_timeout(struct timeval &timeout, int seconds);
void initialize_fd_set(fd_set &read_fds, int client_sock);
bool handle_select_error(int activity);
bool handle_read_data(int client_sock, std::string &total_request, char *buffer, ssize_t &bytes_read);
int read_client_data(int client_sock, std::string &total_request, fd_set &master_set_read, fd_set &master_set_write, std::vector<char> *data);
void init_request(int client_sock, std::string &total_request, bool &keep_alive, const std::vector<server_config>& configs, int config_index, fd_set &master_set_read, fd_set &master_set_write, std::vector<char> *data);
void close_connection(int client_sock, fd_set* master_set_read, fd_set* master_set_write);
void handle_client(int client_sock, fd_set* master_set_read, fd_set* master_set_write, const std::vector<server_config>& configs, int config_index);
void set_default_config_path(std::string& config_path);
bool validate_argument_count(int argc);
bool validate_config_file_extension(const std::string& config_path);
bool initialize_configuration(int argc, char* argv[], std::string& config_path);
bool open_config_file(const std::string& config_path, std::ifstream& config_file);
std::string read_config_file(std::ifstream& config_file);
bool validate_and_tokenize_config(const std::string& content, std::vector<server_config>& configs);
bool load_server_configurations(const std::string& config_path, std::vector<server_config>& configs);
bool configure_socket(Socket* server, const server_config& config, int port);
void add_socket_to_master_set(Socket* server, fd_set& master_set, int& max_fd);
bool setup_servers(const std::vector<server_config>& configs, std::vector<Socket*>& servers, fd_set& master_set, int& max_fd);
int wait_for_activity(fd_set& master_set_read, fd_set& master_set_write, int max_fd);
void process_server_sockets(std::vector<Socket*>& servers, fd_set& master_set_read, int& max_fd, std::vector<int>& client_sockets, fd_set& read_fds, std::map<int, int>& client_to_config_map);
void process_client_sockets(std::vector<int>& client_sockets, std::map<int, std::string>& response_buffers, fd_set& master_set_read, fd_set& master_set_write, fd_set& read_fds, fd_set& write_fds, const std::vector<server_config>& configs, const std::vector<Socket*>& servers, const std::map<int, int>& client_to_config_map);
void run_server_loop(std::vector<Socket*>& servers, fd_set& master_set_read, fd_set& master_set_write, int& max_fd, const std::vector<server_config>& configs);
void cleanup_servers(std::vector<Socket*>& servers);
int is_directory(http_request &request, std::string pathfile);

#endif
