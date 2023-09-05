SRC_DIR	=	src
SRC		=	main.cpp

OBJ_DIR	=	obj
OBJ		=	$(SRC:.cpp=.o)

NAME	=	ircserv

CC		=	c++

CFLAGS	=	-Wall -Wextra -Wpedantic -fno-exceptions -Wpointer-arith -Werror --std=c++98 -Isrc
LDFLAGS =	-Wall -Wextra -Werror

all:		$(NAME)

debug: CFLAGS += -g3 -DDEBUG
debug: re

noerr: CFLAGS += -Wno-error
noerr: all

rel: CFLAGS += -Ofast
rel: LDFLAGS += -O1

$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) -o $(NAME) $(OBJ_DIR)/$(OBJ)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $(SRC_DIR)/$< -o $(OBJ_DIR)/$@ -MMD

clean:
	@-find $(OBJ_DIR) -type f -name '*.o' -delete
	@-find $(OBJ_DIR) -type f -name '*.d' -delete

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re debug noerr rel

-include $(OBJ_DIR)/*.d