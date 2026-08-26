ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME = libft_malloc_$(HOSTTYPE).so
LINK = libft_malloc.so
SRCS =	$(wildcard ./srcs/*.c)
OBJS = $(SRCS:.c=.o)
HEADERS = ./headers/
CFLAGS = -Wall -Wextra -g -fPIC

TEST_NAME = test_malloc
TEST_SRCS = $(wildcard ./tests/*.c)
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_FLAGS = -Wall -Wextra -g

CC = gcc
RM = rm -f

all: $(NAME) $(LINK) $(TEST_NAME)
	@echo
	@printf '\033[32m%s\033[0m\n' "Build complete."
	@echo "> To run the test, execute \"LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so ./test_malloc\""

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -shared -o $(NAME)

$(LINK): $(NAME)
	ln -sf $(NAME) $(LINK)

$(TEST_NAME): $(TEST_OBJS)
	$(CC) $(CFLAGS_TEST) $(TEST_OBJS) -L. -lft_malloc -o $(TEST_NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -I "$(HEADERS)"
				
clean:
	$(RM) $(OBJS) $(TEST_OBJS)
			
fclean:	clean
	$(RM) $(NAME) $(LINK) $(TEST_NAME)
			
re:	fclean all

.PHONY:	all clean fclean re