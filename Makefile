NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread

SRCS_DIR	= coders/srcs
TOOLS_DIR	= coders/tools
OBJS_DIR	= objs
HEADERS_DIR	= coders/inc

SRCS		= $(SRCS_DIR)/main.c \
			  $(SRCS_DIR)/action.c \
			  $(SRCS_DIR)/heap.c \
			  $(SRCS_DIR)/init.c \
			  $(TOOLS_DIR)/cleaner.c \
			  $(TOOLS_DIR)/coder_action.c \
			  $(TOOLS_DIR)/log.c \
			  $(TOOLS_DIR)/parsing.c \
			  $(TOOLS_DIR)/request.c \
			  $(TOOLS_DIR)/utils.c

OBJS		= $(SRCS:%.c=$(OBJS_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I $(HEADERS_DIR) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re