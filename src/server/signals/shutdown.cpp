#include "../../../include/webserv.hpp"

void handle_shutdown_signal() {
    g_exit_flag = 1;
    log_and_print_shutdown_message();
}

void signal_handler(int signal) {
    if (signal == SIGINT) {
        handle_shutdown_signal();
    }
}
