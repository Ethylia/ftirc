SRC_DIR	=	src
SRC		=	main.cpp \
			net/address.cpp \
			net/socket.cpp \
			net/net.cpp \
			server/server.cpp \
			server/client.cpp \

OBJ_DIR	=	obj
OBJ		=	$(addprefix $(OBJ_DIR)/,$(SRC:.cpp=.o))

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

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ -MMD

$(NAME): $(OBJ)
	$(CC) $^ -o $(NAME) $(LDFLAGS)

clean:
	@-find $(OBJ_DIR) -type f -name '*.o' -delete
	@-find $(OBJ_DIR) -type f -name '*.d' -delete

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re debug noerr rel

-include $(OBJ_DIR)/*.d