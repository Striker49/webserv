#ifndef SIGNALS_HPP
#define SIGNALS_HPP

void handle_shutdown_signal();
void signal_handler(int signal);

extern volatile sig_atomic_t g_exit_flag;


#endif 
