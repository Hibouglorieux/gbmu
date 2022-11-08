
NAME = gbmu

FILES = main.cpp \
		Cpu.cpp \
		Mem.cpp \
		Instructions.cpp \
		Clock.cpp


OBJ = $(addprefix obj/,$(FILES:.cpp=.o))

CXXFLAGS = -std=gnu++14 -Wall -Wextra

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $^ -o $@

obj/%.o:src/%.cpp src/*.hpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean :
	rm -rf obj

fclean : clean
	rm -rf $(NAME)

re : fclean all

