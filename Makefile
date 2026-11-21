NAME = codexion
CC = cc
RM = rm -f 
CFLAGS = -Wall -Wextra -Werror -pthread
SRC =	folder_codex/utils.c \
		folder_codex/parsing.c \
		folder_codex/monitor.c \
		folder_codex/main.c \
		folder_codex/init.c \
		folder_codex/heap.c \
		folder_codex/dongle.c \
		folder_codex/coder.c \
		folder_codex/cleanup.c

OBJS = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
		$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
