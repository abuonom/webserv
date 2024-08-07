NAME = webserv

MAKEFLAGS += --silent

SRC =	src/main.cpp \

OBJ = ${SRC:.cpp=.o}

CC = c++

RM = rm -f

CFLAGS = -g -Wall -Wextra -Werror -std=c++98

.o:.cpp
	${CC} ${CFLAGS} -c $< -o ${<:.cpp=.o}

${NAME}:  ${OBJ}
	${CC} $(CFLAGS) ${OBJ} -o $(NAME)
	@echo "\033[32mCompiled OK!\033[0m"

all: ${NAME}

clean:
		${RM} ${OBJ}
		@echo "\033[35mCleared everything!\033[0m"

fclean: clean
		${RM} ${NAME}

re: fclean all
