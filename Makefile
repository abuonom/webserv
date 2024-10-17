NAME = webserv

SRC = $(wildcard src/*.cpp)

OBJ = $(SRC:.cpp=.o)

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -g -std=c++98
#COLORS
RED = \033[1;31m

GREEN = \033[1;32m

YELLOW = \033[1;33m

DEFAULT = \033[0m

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(NAME)
	@echo "$(GREEN)$(NAME) created!$(DEFAULT)"

valgrind : $(NAME)
	valgrind --leak-check=full ./$(NAME)

clean:
	rm -f $(OBJ)
	@echo "$(YELLOW)object files deleted!$(DEFAULT)"

fclean: clean
	rm -f $(NAME)
	@echo "$(RED)all deleted!$(DEFAULT)"

re: fclean all
