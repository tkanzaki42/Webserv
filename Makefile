NAME		= webserv
SRCDIR		= ./srcs
SRCS		= $(shell find $(SRCDIR) -name "*.cpp" -type f | xargs)
OBJS		= $(SRCS:.cpp=.o)
DEPENDS		= $(OBJS:.o=.d)
INCLUDES	= -I./includes -I./
CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -pedantic-errors -MMD -MP

.PHONY: all
all: $(NAME)

-include $(DEPENDS)

.cpp.o: $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(NAME) $(OBJS)

.PHONY: clean
clean:
	$(RM) $(OBJS) $(DEPENDS)

.PHONY: fclean
fclean: clean
	$(RM) $(NAME)

.PHONY: re
re: fclean all

.PHONY: debug
debug: 
ifeq ($(shell uname), Darwin)
	# mac
debug: CXXFLAGS += -g -fsanitize=integer -fsanitize=undefined -DDEBUG
else
	#Linux and others...
debug: CXXFLAGS += -g -fsanitize=integer -fsanitize=address -fsanitize=leak -fsanitize=undefined -DDEBUG
endif
debug: CXX = clang++
debug: re

.PHONY: usage
usage:
	@echo "Available options for make:"
	@echo "  make"
	@echo "  make all       Build everything."
	@echo "  make clean"
	@echo "  make flean"
	@echo "  make re"
	@echo "  make debug     Build with fsanitize options."

stestdir  = ./stest
stestfile = result.log
stest     = $(stestdir)/$(stestfile)
.PHONY: siege
siege:
	siege -t 1m -b http://localhost:5050/empty.html
