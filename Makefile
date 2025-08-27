CPP = c++
SRC = src/main.cpp src/Utils.cpp src/Document.cpp
CFLAGS = -Wall -Werror -Wextra

NAME = webserv

OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME):
	$(CPP) $(CFLAGS) $(SRC) -o $(NAME)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re