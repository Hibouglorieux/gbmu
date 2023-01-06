ifndef ECHO
T					:=	$(shell $(MAKE) $(MAKECMDGOALS) --no-print-directory \
	  -nrRf $(firstword $(MAKEFILE_LIST)) \
	  ECHO="OutputPrintable" | grep -c "OutputPrintable")
N					:=	x
C					=	$(words $N)$(eval N := x $N)
V					=	"`expr "   [\`expr $C '*' 100 / $T\`" : '.*\(....\)$$'`%]"
ECHO				=	printf "\e[1A\r%s %s %s                                          \n" $V
endif

# Color

RESET_C				=	\033[m
B_C					=	\033[0;34m
Y_C					=	\033[0;33m
G_C					=	\033[0;32m
R_C					=	\033[0;31m
M_C					=	\033[0;35m
C_C					=	\033[0;36m
F_C					=	\033[35;5;108m

# Programs names

NAME				=	gbmu
LNAME				=	libimgui.a

# Dir/Files Path (Do not modify)

S_PATH				=	src/
H_PATH				+=	src/
L_PATH				=	src/imgui/
B_PATH				=	.build/
O_PATH				=	.build/objs/


###############################################################################
#                               Modifications                                 #
###############################################################################

# Add custom dir for .o
IMGUI				=	imgui/

# Add previous custom dir with $(O_PATH){custom dir} to PATH variable

PATHS				+=	$(B_PATH)
PATHS				+=	$(O_PATH)
PATHS				+=	$(O_PATH)$(IMGUI)

# Files

SRC					+= $(S_PATH)Clock.cpp
SRC					+= $(S_PATH)Cpu.cpp
SRC					+= $(S_PATH)CpuStackTrace.cpp
SRC					+= $(S_PATH)CpuUtility.cpp
SRC					+= $(S_PATH)Debugger.cpp
SRC					+= $(S_PATH)FlagOp.cpp
SRC					+= $(S_PATH)Gameboy.cpp
SRC					+= $(S_PATH)Instructions8BitArithmetic.cpp
SRC					+= $(S_PATH)Instructions8BitLoad.cpp
SRC					+= $(S_PATH)Instructions8BitShift.cpp
SRC					+= $(S_PATH)Instructions16BitArithmetic.cpp
SRC					+= $(S_PATH)Instructions16BitLoad.cpp
SRC					+= $(S_PATH)InstructionsControl.cpp
SRC					+= $(S_PATH)InstructionsJumpCalls.cpp
SRC					+= $(S_PATH)InstructionsMisc.cpp
SRC					+= $(S_PATH)Joypad.cpp
SRC					+= $(S_PATH)Loop.cpp
SRC					+= $(S_PATH)main.cpp
SRC					+= $(S_PATH)MBC.cpp
SRC					+= $(S_PATH)Mem.cpp
SRC					+= $(S_PATH)Ppu.cpp
SRC					+= $(S_PATH)Screen.cpp
SRC					+= $(S_PATH)Sprite.cpp
SRC					+= $(S_PATH)TilePixels.cpp
SRC					+= $(S_PATH)Hdma.cpp

# Header

IMGUIHDR			+=	$(IMGUI)imconfig.h
IMGUIHDR			+=	$(IMGUI)imgui.h
IMGUIHDR			+=	$(IMGUI)imgui_impl_sdl.h
IMGUIHDR			+=	$(IMGUI)imgui_impl_sdlrenderer.h
IMGUIHDR			+=	$(IMGUI)imgui_internal.h
IMGUIHDR			+=	$(IMGUI)imgui_stdlib.h
IMGUIHDR			+=	$(IMGUI)imstb_rectpack.h
IMGUIHDR			+=	$(IMGUI)imstb_textedit.h
IMGUIHDR			+=	$(IMGUI)imstb_truetype.h

HDR					+=	Clock.hpp
HDR					+=	Cpu.hpp
HDR					+=	CpuStackTrace.hpp
HDR					+=	Debugger.hpp
HDR					+=	define.hpp
HDR					+=	Gameboy.hpp
HDR					+=	Instructions.hpp
HDR					+=	Joypad.hpp
HDR					+=	Loop.hpp
HDR					+=	MBC.hpp
HDR					+=	Mem.hpp
HDR					+=	Ppu.hpp
HDR					+=	Screen.hpp
HDR					+=	Sprite.hpp
HDR					+=	TilePixels.hpp
HDR					+=	Utility.hpp
HDR					+=	Hdma.hpp
HDR					+=	Utility.tpp

###############################################################################
#                                                                             #
###############################################################################

# Objects

OBJ					=	$(patsubst $(S_PATH)%.cpp, $(O_PATH)%.o, $(SRC))

LIB					=	$(LNAME)
vpath %.h $(H_PATH)
vpath %.hpp $(H_PATH)
vpath %.tpp $(H_PATH)

# Variables

DEBUG				=
CFLAGS				=	-Wall -Wextra -std=gnu++14 -Og -g3 #-Werror
ifeq ($(DEBUG), g)
	CFLAGS			=	-g
else ifeq ($(DEBUG), fsanitize)
	CFLAGS			=	-fsanitize=address -g3
else ifeq ($(DEBUG), hard)
	CFLAGS			+=	-Wall -Weverything -fsanitize=address,undefined -Wno-cast-qual
	CFLAGS			+=	-Wno-missing-noreturn -Wno-disabled-macro-expansion
else ifeq ($(DEBUG), dev)
	CFLAGS			=
endif

CC					=	g++ $(CFLAGS)
IFLAGS				+=	$(addprefix -I, $(H_PATH))
CMPLC				=	$(CC) -c $(IFLAGS)
CMPLO				=	$(CC) -o
BUILD				=	$(PATHS)
AR_RC				=	ar rc
RANLI				=	ranlib
RM_RF				=	/bin/rm -rf
MKDIR				=	mkdir -p
SLEEP				=	sleep 0.01

.PHONY: all norme clean fclean re

# Rules
make:
	$(MAKE) --no-print-directory all

all:  $(BUILD) $(NAME)

# Compilation core

ifneq ("$(wildcard$(LNAME))","")
TEST				=	libimgui.a
else
TEST				=
endif

$(NAME): $(OBJ) $(TEST)
	make -C $(L_PATH)
	$(ECHO) $(GCFIL) $(NAME)
	$(CMPLO) $(NAME) $(OBJ) $(LIB) `sdl2-config --cflags --static-libs`
	$(GCSUC)
	echo "---\nCFLAGS - =$(B_C) $(CFLAGS)$(RESET_C)\n---"
	cp $(NAME) $(B_PATH)$(NAME)

$(OBJ): $(O_PATH)%.o: $(S_PATH)%.cpp $(HDR) $(IMGUIHDR)
	$(CMPLC) $< -o $@ `sdl2-config --cflags --static-libs`
	$(ECHO) $(GCFIL) $<

$(PATHS):
	$(MKDIR) $(PATHS)

clean:
	for i in $(OBJ); do $(RM_RF) $$i; $(ECHO) $(RMSHW) $$i; done
	$(CLSUC)

fclean:	clean
	for i in $(OBJ); do $(RM_RF) $$i; $(ECHO) $(RMSHW) $$i; done
	for i in $(PATHS); do $(RM_RF) $$i; $(ECHO) $(RMSHW) $$i; done
	$(RM_RF) $(NAME)
	$(ECHO) $(RMSHW) $(NAME)
	$(CLSUC)
re:
	$(MAKE) --no-print-directory fclean all

ifndef VERBOSE
.SILENT:
endif
