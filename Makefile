ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LINK = libft_malloc.so
SRCS =	$(wildcard ./srcs/*.c)
OBJS = $(SRCS:.c=.o)
HEADERS = ./headers/

TEST_NAME = test_malloc.out
TEST_SRCS = $(wildcard ./tests/*.c)
TEST_OBJS = $(TEST_SRCS:.c=.o)

CC = gcc
CFLAGS = -Wall -Wextra -g -fPIC
RM = rm -f

all: $(NAME) $(LINK) $(TEST_NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -shared -o $(NAME) -I "$(HEADERS)"

$(LINK): $(NAME)
	ln -sf $(NAME) $(LINK)

$(TEST_NAME): $(TEST_OBJS)
	$(CC) $(CFLAGS) $(TEST_OBJS) -o $(TEST_NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I "$(HEADERS)"
				
clean:
	$(RM) $(OBJS) $(TEST_OBJS)
			
fclean:	clean
	$(RM) $(NAME) $(LINK) $(TEST_NAME)
			
re:	fclean all

.PHONY:	all clean fclean re