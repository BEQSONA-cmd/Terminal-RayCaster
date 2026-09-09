NAME = game 
CC = cc
OBJ = $(SRC:.c=.o)

LFLAGS = -lm -lX11
SRC = player.c utils.c main.c

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(SRC) -o $(NAME) $(LFLAGS)

re:
	make fclean
	make all

fclean:
	rm -rf $(OBJ)
	rm -rf $(NAME)
