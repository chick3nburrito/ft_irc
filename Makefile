NAME = ircserv

BONUS_NAME = bot

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS = main.cpp \
		srcs/Server.cpp \
		srcs/Client.cpp \
		srcs/Channel.cpp \
		srcs/Parser.cpp \
		srcs/ToolsHelper.cpp \
		srcs/CommandDispatcher.cpp \
		srcs/commands/Pass.cpp \
		srcs/commands/Nick.cpp \
		srcs/commands/User.cpp \
		srcs/commands/Join.cpp \
		srcs/commands/PrivMsg.cpp \
		srcs/commands/Topic.cpp \
		srcs/commands/Kick.cpp \
		srcs/commands/Invite.cpp \
		srcs/commands/Mode.cpp \
		srcs/Replies.cpp

BONUS_SRCS = ./bonus/Bot.cpp srcs/Parser.cpp srcs/ToolsHelper.cpp

OBJS = $(SRCS:.cpp=.o)

BONUS_OBJS = $(BONUS_SRCS:.cpp=.o)

RM = rm -f

all : $(NAME)

bonus : all $(BONUS_NAME)

$(NAME) : $(OBJS)
		$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(BONUS_NAME) : $(BONUS_OBJS)
			$(CXX) $(CXXFLAGS) $(BONUS_OBJS) -o $(BONUS_NAME)

%.o : %.cpp
		$(CXX) $(CXXFLAGS) -c $< -o $@

clean :
		$(RM) $(OBJS) $(BONUS_OBJS)

fclean : clean
		$(RM) $(NAME) $(BONUS_NAME)

re : fclean all

.PHONY : all clean fclean re bonus