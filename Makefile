NAME = minishell

PREFIX = $(shell find ${HOME} -name aeloyan-readline 2>/dev/null)

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
	$(CC) $(OBJS) -L. -lft  -L./aeloyan-readline/lib  -lreadline -I./aeloyan-readline/include -o $(NAME)

all: $(NAME)

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME) libft.a

readline:
	cd readline-master  && ./configure --prefix=$(PREFIX) && make && make install

re: fclean all

.PHONY: $(NAME) all clean fclean re
