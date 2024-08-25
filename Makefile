NAME		=	ircserv

SRCS_DIR	=	srcs
OBJS_DIR	=	objs

SRCS		=	cpp/IRCClient.cpp \
				cpp/IRCServer.cpp \
				cpp/IRCCmds.cpp \
				cpp/IRCChannel.cpp \
				cpp/main.cpp

OBJS		=	$(patsubst $(SRCS_DIR)/%.cpp,$(OBJS_DIR)/%.o,$(SRCS))

RM			=	rm -rf
CXX			=	c++
FLAGS		=	-I./hpp -Wall -Wextra -Werror -std=c++98 -g3

$(NAME):	$(OBJS)
	$(CXX) $(FLAGS) $^ -o $@

$(OBJS_DIR)/%.o:	$(SRCS_DIR)/%.cpp | $(OBJS_DIR)
	$(CXX) $(FLAGS) -c $< -o $@

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

all:	$(NAME)

clean:
	$(RM) $(OBJS_DIR)

fclean:	clean
	$(RM) $(NAME)

re:	fclean all

.PHONY:	all clean fclean re