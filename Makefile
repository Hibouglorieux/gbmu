
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
		Gameboy.cpp \
		TilePixels.cpp \
		CpuStackTrace.cpp \
		MBC.cpp \
		Sprite.cpp \
		Debugger.cpp



IMGUI_PATH = src/imgui/
LIBS = libimgui.a
OBJ = $(addprefix obj/,$(FILES:.cpp=.o))

CXXFLAGS = -std=gnu++14 -Wall -Wextra -g3 -Og

all: $(NAME)

$(NAME): $(OBJ)
	make -C $(IMGUI_PATH)
	$(CXX) $^ -o $@ `sdl2-config --cflags --libs` $(LIBS)



obj/%.o:src/%.cpp src/*.hpp src/define.hpp src/*.tpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean :
	@#make -C $(IMGUI_PATH) clean
	rm -rf obj

fclean : clean
	@#make -C $(IMGUI_PATH) fclean
	rm -rf $(NAME)

re : fclean all

.PHONY: all norme clean fclean re
