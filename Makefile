SRCS =	server.cpp\
		functions.cpp\
		main.cpp\
		user.cpp\
		commands.cpp\
		channel.cpp\
		



NAME = server

CC = c++

FLAGS = -Wall -Werror -Wextra -std=c++98 -g3 -fsanitize=address

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
		@echo $(OBJS)
			$(CC) $(FLAGS) -o $(NAME) $(OBJS)

$(OBJS): $(SRCS)
	$(CC) $(FLAGS) -c $(SRCS)

clean:
		rm -rf $(OBJS)

fclean:		clean
		rm -rf $(NAME)

re:		fclean $(NAME)

.PHONY: all re clean fclean