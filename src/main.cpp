#include "../include/webserv.hpp"

logger log_instance("logs/server.log");
volatile sig_atomic_t g_exit_flag = 0;

int main(int argc, char* argv[]) {
    signal(SIGINT, signal_handler);
    std::string config_path;

    if (!initialize_configuration(argc, argv, config_path)) {
        return 1;
    }
    std::vector<server_config> configs;
    if (!load_server_configurations(config_path, configs)) {
        return 1;
    }
    std::vector<Socket*> servers;
    fd_set master_set_read;
    fd_set master_set_write;
    FD_ZERO(&master_set_read);
    FD_ZERO(&master_set_write);
    int max_fd = 0;

    if (!setup_servers(configs, servers, master_set_read, max_fd)) {
        print_message(STDERR_FILENO, "Error: Server setup failed.", COLOR_RED);
        return 1;
    }
    log_instance.info("All servers are running. Press Ctrl+C to stop.");
    print_message(STDERR_FILENO, "All servers are running. Press Ctrl+C to stop.", COLOR_MAGENTA);
    run_server_loop(servers, master_set_read, master_set_write, max_fd, configs);
    cleanup_servers(servers);
    log_instance.info("Servers shut down successfully.");
    print_message(STDERR_FILENO, "Servers shut down successfully.", COLOR_MAGENTA);
    return 0;
}
