#--- PROGRAM NAME ---#

NAME	=	webserv

#--- COLORS ---#

RED		= \033[1;31m
GREEN	= \033[1;32m
BLUE	= \033[1;34m
YELLOW	= \033[1;33m
DARKGRAY= \033[1;30m
DEFAULT = \033[0m

#--- LIBRARIES AND HEADERS ---#

HEADER_FILES	=	webserv.hpp utils/logger.hpp utils/colors.hpp utils/utils.hpp \
					server/config/config_parser.hpp server/core/server.hpp \
					sockets/sockets.hpp sockets/socket_manager.hpp \
					client/client.hpp client/client_manager.hpp client/connection/client_connection.hpp client/request/client_request.hpp client/response/client_response.hpp
HEADERS			=	$(addprefix $(INCDIR)/, $(HEADER_FILES))

LIB			=	

#--- COMMAND VARIABLES ---#
OS = $(shell uname -s) 

CC		=	c++


# ifeq ($(DEBUG), true)
ifeq ($(OS),Linux)
	CFLAGS = -Wall -Wextra -Werror
endif
ifeq ($(OS),Darwin)
	CFLAGS = -Wall -Werror -Wextra -std=c++98 -pedantic 
#CFLAGS = -Wall -Werror -Wextra -std=c++98 -pedantic-errors -nostdinc++ -nostdlib++ -I/path/to/c++98/includes -L/path/to/c++98/libs
endif

MK		=	mkdir -p
RM		=	rm -rf

#--- SOURCE, INCLUDE, LIBRARY AND BINARIES DIRECTORIES ---#

INCDIR	=	include
LIBDIR	=	libs
SRCDIR	=	src
BINDIR	=	bin

#--- SOURCES ---#

SRCS	=	main.cpp utils/logger.cpp utils/print_servers.cpp server/config/config_parser.cpp server/config/locations.cpp server/core/server.cpp sockets/sockets.cpp sockets/socket_manager.cpp client/client.cpp client/client_manager.cpp client/connection/client_connection.cpp client/request/client_request.cpp client/response/client_response.cpp
SRC		=	$(addprefix $(SRCDIR)/, $(SRCS))
BIN     =	$(patsubst $(SRCDIR)/%.cpp,$(BINDIR)/%.o,$(SRC))

#--- RULES ---#

$(BINDIR)/%.o	: $(SRCDIR)/%.cpp $(HEADERS)
	@$(MK) $(@D)
	@echo "${DARKGRAY}Compiling : $(@F) ... ${DEFAULT}"
	@$(CC) $(CFLAGS) -c $< -o $@

all			: ${NAME}

${NAME}		: ${BIN}
	@$(CC) ${CFLAGS} ${BIN} -o ${NAME}
	@echo "${GREEN}${NAME} successfully created. 🌐${DEFAULT}"

clean		:
	@${RM} ${BINDIR}
	@echo "${YELLOW}Binary files successfully removed 🗑${DEFAULT}"

fclean		: clean
	@${RM} ${NAME}
	@echo "${RED}Executable successfully removed 🗑${DEFAULT}"

re	 		: fclean all

leaks   :
	@tests/leakcheck.sh

#--- PHONY ---#

.PHONY	 	: all clean fclean re leak_test