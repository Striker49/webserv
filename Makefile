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

HEADER_FILES	=	webserv.hpp \
					cgi/cgi.hpp \
					server/config/parser.hpp \
					server/core/core.hpp \
					server/handlers/locations.hpp \
					server/signals/signals.hpp \
					server/server.hpp \
					sockets/sockets.hpp \
					http/request/http_request.hpp \
					http/response/http_response.hpp \
					http/methods/http_methods.hpp \
					utils/debug/colors.hpp utils/logging/logger.hpp 
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
	CFLAGS = -Wall -Werror -Wextra -std=c++98 -pedantic -fsanitize=address -g
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

SRCS	=	main.cpp \
			cgi/cgi.cpp \
			server/config/parser.cpp \
			server/core/core.cpp \
			server/handlers/locations.cpp \
			server/signals/shutdown.cpp \
			server/server.cpp \
			sockets/sockets.cpp \
			http/request/http_request.cpp \
			http/response/http_response.cpp \
			http/methods/http_methods.cpp \
			utils/debug/print_servers.cpp \
			utils/debug/cout_msg.cpp \
			utils/logging/logger.cpp

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