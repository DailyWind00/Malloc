ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LINK = libft_malloc.so

SRCS =	$(wildcard ./srcs/*.c)
OBJS = $(SRCS:.c=.o)
HEADERS = ./headers/

CC = gcc
CFLAGS = -Wall -Wextra -g -fPIC
RM = rm -f

all:			$(NAME) $(LINK)

$(NAME):		$(OBJS)
				$(CC) $(CFLAGS) $(OBJS) -shared -o $(NAME) -I "$(HEADERS)"

$(LINK):		$(NAME)
				ln -sf $(NAME) $(LINK)

%.o: 			%.c
				$(CC) $(CFLAGS) -c $< -o $@ -I "$(HEADERS)"
				
clean:
			$(RM) $(OBJS)
			
fclean:		clean
			$(RM) $(NAME) $(LINK)
			
re:			fclean all

.PHONY:		all clean fclean re