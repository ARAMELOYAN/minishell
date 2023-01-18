NAME = minishell

SRCS = $(wildcard *.c)

OBJS = $(SRCS:.c=.o)

CC = cc

FLAGS = -Wall -Wextra -Werror 

RM = rm -rf

.c.o:
	$(CC) -c $< -o $(<:.c=.o)

$(NAME): $(OBJS)
	$(MAKE) bonus -C ./libft
	mv libft/libft.a .
	$(MAKE) -C ./libft fclean
	$(CC) $(OBJS) libft.a -lreadline -o $(NAME)

all: $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME) libft.a

re: fclean all

.PHONY: $(NAME) all clean fclean re
