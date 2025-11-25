# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aruckenb <aruckenb@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/11/18 15:34:59 by krabitsc          #+#    #+#              #
#    Updated: 2025/11/24 15:59:20 by aruckenb         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	 	= ircserv

SRCS 	 	= srcs/main.cpp \
			  srcs/Server.cpp \
			  srcs/Parser.cpp
			  srcs/Client.cpp \
			  srcs/Channel.cpp

INCLUDES    = -I ./includes	  
			  
OBJDIR 		= build
#OBJS 		= $(addprefix $(OBJDIR)/, $(/SRCS:.cpp=.o))
OBJS        = $(SRCS:srcs/%.cpp=$(OBJDIR)/%.o)
DEPS 		= $(OBJS:.o=.d)
CC 	 		= c++
#CPPFLAGS   	= -Wall -Wextra -Werror -std=c++98 -MMD -MP $(INCLUDES)
CPPFLAGS   	= -std=c++98 -MMD -MP $(INCLUDES)
CPPFLAGSDBG = $(CPPFLAGS) -FT_IRC_DEBUG
BUILD_FLAGS ?= $(CPPFLAGS)
RMF		 	= rm -f
MKDIR 		= mkdir -p

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(BUILD_FLAGS) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: srcs/%.cpp
	$(MKDIR) $(OBJDIR)
	$(CC) $(BUILD_FLAGS) -c $< -o $@

clean:
	$(RMF) $(OBJS) $(DEPS)
	$(RMF) -r $(OBJDIR)

fclean: clean
	$(RMF) $(NAME)

re: fclean all

debug:
	$(MAKE) clean
	$(MAKE) BUILD_FLAGS="$(CPPFLAGSDBG)"

-include $(DEPS)

.PHONY: all clean fclean re debug