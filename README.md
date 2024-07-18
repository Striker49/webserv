# WEBSERV

## Introduction

## Project Guidelines

## Project Scale

# DEEP DIVE

## Infrastructure

Using the command `tree` we can display in the console the folder tree of the working directory. The following structure aims to separate the main elements that compose an http server.

```
.
├── Makefile
├── README.md
├── configs
│   ├── default.conf
│   ├── empty.conf
│   ├── multiserv.conf
│   ├── threejsserv.conf
│   └── wrong.conf
├── include
│   ├── cgi
│   │   └── cgi.hpp
│   ├── client
│   │   ├── client.hpp
│   │   ├── connection
│   │   │   └── client_connection.hpp
│   │   ├── request
│   │   │   └── client_request.hpp
│   │   └── response
│   │       └── client_response.hpp
│   ├── http
│   │   ├── http.hpp
│   │   ├── methods
│   │   │   └── http_methods.hpp
│   │   ├── request
│   │   │   └── http_request.hpp
│   │   └── response
│   │       └── http_response.hpp
│   ├── server
│   │   ├── config
│   │   │   └── parser.hpp
│   │   ├── core
│   │   │   └── core.hpp
│   │   ├── handlers
│   │   │   ├── handlers.hpp
│   │   │   └── locations.hpp
│   │   └── server.hpp
│   ├── sockets
│   │   └── sockets.hpp
│   ├── utils
│   │   ├── debug
│   │   │   └── colors.hpp
│   │   ├── logging
│   │   │   └── logger.hpp
│   │   └── utils.hpp
│   └── webserv.hpp
├── logs
│   └── server.log
├── src
│   ├── cgi
│   │   └── cgi.cpp
│   ├── client
│   │   ├── client.cpp
│   │   ├── connection
│   │   │   └── client_connection.cpp
│   │   ├── request
│   │   │   └── client_request.cpp
│   │   └── response
│   │       └── client_response.cpp
│   ├── http
│   │   ├── http.cpp
│   │   ├── methods
│   │   │   └── http_methods.cpp
│   │   ├── request
│   │   │   └── http_request.cpp
│   │   └── response
│   │       └── http_response.cpp
│   ├── main.cpp
│   ├── server
│   │   ├── config
│   │   │   └── parser.cpp
│   │   ├── core
│   │   │   └── core.cpp
│   │   ├── handlers
│   │   │   └── locations.cpp
│   │   └── server.cpp
│   ├── sockets
│   │   └── sockets.cpp
│   └── utils
│       ├── debug
│       │   ├── cout_msg.cpp
│       │   └── print_servers.cpp
│       └── logging
│           └── logger.cpp
├── tests
│   ├── client
│   └── scripts
└── www
    ├── public
    │   ├── assets
    │   │   └── images
    │   ├── css
    │   ├── html
    │   │   └── index.html
    │   └── javascript
    │       └── mian.js
    └── src
        └── pages
```

51 directories, 47 files

## Configuration

Using the ngnix configuration file as base model our program takes as first and only argument a `.conf` file containing the attributes of a server or multiples.

`default.conf` exemple :

```
server {
	listen = 2323;
    	server_name = webserv;
	root = www/;
	error_pages = 400, www/error_pages/400.html;
	error_pages = 401, www/error_pages/401.html;
	error_pages = 404, www/error_pages/404.html;
	max_body_size = 600M;
	upload_path = ./www/upload/;
	autoindex = true;
	allowed_methods = GET POST DELETE;
 	location / {
		root = ./www/;
    }
	location /pages {
		root = ./www/page;
		index = index.html;
	}
	location /cookies {
		root = ./www/cookies/;
		index = index.html;
	}
	location /pages/pong {
		root = ./www/page/pong/;
		index = pong.html;
	}
	location /err {
		root = ./www/error_pages/;
		autoindex = true;
	}
	location /teapot {
		root =  301, error_pages/418.html;
	}

	location /upload {
		root = ./www/upload/;
		autoindex = true;
	}

	location /cgi {
		cgi = .php, /usr/bin/php;
		cgi = .py, /usr/bin/python3;
		cgi = .sh, /bin/bash;
		root = ./www/cgi/;
	}
}

```

As we can see, the file declares various essential caracteristics of a web server.

The core idea is the basic format of a server block delimited by `server { ... }` and the presence of ` keys = values;`

There are other complementary caracteristics like `autoindex` and `location`. Every `key` has a required format and an expected `value`.

For further testing, we can find in the `./configs` folder different `.conf` files that isolate other pausibles entries.

### Header Files

The following header structure shows an attemp of creating a concise hierarchy by grouping every submodule into it's `.hpp` parent. By doing so `webserv.hpp` only contains the parent modules and can be called everywhere and efficiently throughout the sources files.

```
├── include
│   ├── cgi
│   │   └── cgi.hpp
│   ├── client
│   │   ├── client.hpp
│   │   ├── connection
│   │   │   └── client_connection.hpp
│   │   ├── request
│   │   │   └── client_request.hpp
│   │   └── response
│   │       └── client_response.hpp
│   ├── http
│   │   ├── http.hpp
│   │   ├── methods
│   │   │   └── http_methods.hpp
│   │   ├── request
│   │   │   └── http_request.hpp
│   │   └── response
│   │       └── http_response.hpp
│   ├── server
│   │   ├── config
│   │   │   └── parser.hpp
│   │   ├── core
│   │   │   └── core.hpp
│   │   ├── handlers
│   │   │   ├── handlers.hpp
│   │   │   └── locations.hpp
│   │   └── server.hpp
│   ├── sockets
│   │   └── sockets.hpp
│   ├── utils
│   │   ├── debug
│   │   │   └── colors.hpp
│   │   ├── logging
│   │   │   └── logger.hpp
│   │   └── utils.hpp
│   └── webserv.hpp
```

The main header `webserv.hpp` will also include all the external and system librairies. We can then call `#include "../path/to/include/webserv.hpp"` and be certain of all dependecies.

### Program Environment

The program uses a series of utils to ensure quality of life and robust debbuging. The main is a logging tool to monitor the server's activity and execution flow.

The first thing that follows the `#include` is a `logger log_instance("logs/server.log");`. This will initiate the monitoring of our server and it will be written in the `server.log` file.

Then we declare a goblal variable `volatile sig_atomic_t g_exit_flag = 0;` that has as purpose to intercept the ctrl+c signal defined in `<csignal>` by :

`2     SIGINT           terminate process    interrupt program`

The `volatile` keyword is used in C++ (and C) to indicate that a variable's value can be changed at any time without any action being taken by the code the compiler finds nearby. In practical terms, it prevents the compiler from applying any optimizations on the variable that assume values cannot change between two accesses to the variable. This is important for variables that may be modified by external events such as hardware interrupts, other threads, or signal handlers.

`sig_atomic_t` is a typedef provided by C and C++ standard libraries. It defines an integer type that can be accessed atomically, meaning that operations on this type can be completed in a single instruction cycle. Accessing or modifying these variables is guaranteed to not be interrupted by signals — a property crucial for communicating between a signal handler and the main program or other parts of a program asynchronously.

We will use the goblal variable as a boolean flag to catch any signal teminating the process and gracefully destroying and shutting down our program.

### Parsing

The program starts by setting up the signal handler.

```
int main(int argc, char* argv[]) {
    signal(SIGINT, signal_handler);

	...
```

Since the program can be terminated at any moment this is the first initialisation we have to do.

The second step is validating the configuration file. The declaration on the string type `config_path` variable will be used to store the path of the file.

```
    std::string config_path;
    if (!initialize_configuration(argc, argv, config_path)) {
        return 1;
    }
```

Using a boolean the `initialize_configuration(argc, argv, config_path)` function will return `1` if the parsing is successful.

```
bool initialize_configuration(int argc, char* argv[], std::string& config_path) {
    config_path = "configs/default.conf";

    if (argc > 2) {
        log_instance.error("Usage: ./webserv <config_file>");
        print_message(STDERR_FILENO, "Usage: ./webserv <config_file>", COLOR_RED);
        return false;
    } else if (argc == 2) {
        config_path = argv[1];
        if (config_path.size() < 5 || config_path.substr(config_path.size() - 5) != ".conf") {
            log_instance.error("Configuration file must have a .conf extension");
            print_message(STDERR_FILENO, "Error: Configuration file must have a .conf extension", COLOR_RED);
            return false;
        }
    }
    return true;
}
```

There are three verifications in this routine.

- The first is one is if `argv[1] == NULL` we insert the default configuration file path.
- The second is if there is more than 2 arguments passed as parameters, we display a usage message.
- The thrid is if there is a value given in `argv[1]`, we proceed to verify the extention by counting if the string is less than 5 characters, meaning shorter than ".conf" or by substituting the string by counting 5 characters from the end and comparing to it ".conf". We can only accept ".conf" extentions.
