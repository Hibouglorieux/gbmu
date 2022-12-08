
NAME = gbmu

FILES = main.cpp \
		Cpu.cpp \
		CpuUtility.cpp \
		Mem.cpp \
		InstructionsControl.cpp \
		InstructionsJumpCalls.cpp \
		Instructions8BitLoad.cpp \
		Instructions16BitLoad.cpp \
		Instructions16BitArithmetic.cpp \
		Instructions8BitArithmetic.cpp \
		Instructions8BitShift.cpp \
		InstructionsMisc.cpp \
		FlagOp.cpp \
		Clock.cpp \
		Screen.cpp \
		Loop.cpp \
		Ppu.cpp \
		Joypad.cpp \
		Gameboy.cpp


OBJ = $(addprefix obj/,$(FILES:.cpp=.o))

CXXFLAGS = -std=gnu++14 -Wall -Wextra -g

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $^ -o $@ `sdl2-config --cflags --libs`

obj/%.o:src/%.cpp src/*.hpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean :
	rm -rf obj

fclean : clean
	rm -rf $(NAME)

re : fclean all

