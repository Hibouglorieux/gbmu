
# Progress bar with on line output printing. This script get the maximum
# lines , then expr a compilation percentage.
ifneq ($(words $(MAKECMDGOALS)),1)
.DEFAULT_GOAL		=	make
%:
	  @$(MAKE) $@ --no-print-directory -rRf $(firstword $(MAKEFILE_LIST))
else

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
H_PATH				+=	includes/
B_PATH				=	build/
O_PATH				=	build/objs/

###############################################################################
#                               Modifications                                 #
###############################################################################

# Add custom dir for .o
CPU				=	CPU/
DBG				=	debugger/
IMGUI			=	imgui/
FONTS			=	fonts/


# Add previous custom dir with $(O_PATH){custom dir} to PATH varriable

PATHS				+=	$(B_PATH)
PATHS				+=	$(O_PATH)
PATHS				+=	$(O_PATH)$(CPU)
PATHS				+=	$(O_PATH)$(DBG)
PATHS				+=	$(O_PATH)$(IMGUI)
#PATHS				+=	$(O_PATH)$(IMGUI)$(FONTS)


# Files

SRC					+= $(S_PATH)Clock.cpp
SRC					+= $(S_PATH)Gameboy.cpp
SRC					+= $(S_PATH)Joypad.cpp
SRC					+= $(S_PATH)Loop.cpp
SRC					+= $(S_PATH)main.cpp
SRC					+= $(S_PATH)Mem.cpp
SRC					+= $(S_PATH)Ppu.cpp
SRC					+= $(S_PATH)Screen.cpp

SRC					+= $(S_PATH)$(CPU)Cpu.cpp
SRC					+= $(S_PATH)$(CPU)CpuUtility.cpp
SRC					+= $(S_PATH)$(CPU)FlagOp.cpp
SRC					+= $(S_PATH)$(CPU)Instructions8BitArithmetic.cpp
SRC					+= $(S_PATH)$(CPU)Instructions8BitLoad.cpp
SRC					+= $(S_PATH)$(CPU)Instructions8BitShift.cpp
SRC					+= $(S_PATH)$(CPU)Instructions16BitArithmetic.cpp
SRC					+= $(S_PATH)$(CPU)Instructions16BitLoad.cpp
SRC					+= $(S_PATH)$(CPU)InstructionsControl.cpp
SRC					+= $(S_PATH)$(CPU)InstructionsJumpCalls.cpp
SRC					+= $(S_PATH)$(CPU)InstructionsMisc.cpp

SRC					+= $(S_PATH)$(DBG)Debugger.cpp

SRC					+= $(S_PATH)$(IMGUI)imgui.cpp
SRC					+= $(S_PATH)$(IMGUI)imgui_demo.cpp
SRC					+= $(S_PATH)$(IMGUI)imgui_draw.cpp
SRC					+= $(S_PATH)$(IMGUI)imgui_impl_sdl.cpp
SRC					+= $(S_PATH)$(IMGUI)imgui_impl_sdlrenderer.cpp
SRC					+= $(S_PATH)$(IMGUI)imgui_stdlib.cpp
SRC					+= $(S_PATH)$(IMGUI)imgui_tables.cpp
SRC					+= $(S_PATH)$(IMGUI)imgui_widgets.cpp


# Headers

HDR					+=	Clock.hpp
HDR					+=	Cpu.hpp
HDR					+=	Debugger.hpp
HDR					+=	Gameboy.hpp
HDR					+=	imconfig.h
HDR					+=	imgui.h
HDR					+=	imgui_impl_sdl.h
HDR					+=	imgui_impl_sdlrenderer.h
HDR					+=	imgui_internal.h
HDR					+=	imgui_stdlib.h
HDR					+=	imstb_rectpack.h
HDR					+=	imstb_textedit.h
HDR					+=	imstb_truetype.h
HDR					+=	Instructions.hpp
HDR					+=	Joypad.hpp
HDR					+=	Loop.hpp
HDR					+=	Mem.hpp
HDR					+=	Ppu.hpp
HDR					+=	Screen.hpp
HDR					+=	Utility.hpp
HDR					+=	Utility.tpp

###############################################################################
#                                                                             #
###############################################################################

# Objects

OBJ					=	$(patsubst $(S_PATH)%.cpp, $(O_PATH)%.o, $(SRC))
LIB					=	$(LNAME)
#VPATH				=	includes/
vpath %.h $(H_PATH)
vpath %.hpp $(H_PATH)
vpath %.tpp $(H_PATH)

# Variables

DEBUG				=
CFLAGS				=	-Wall -Wextra -Werror -std=gnu++14
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
GCFIL				=	"- > Compiling	-"
RMSHW				=	"- - Removing	-"
MKSHW				=	"- + Creating	-"
GCSUC				=	echo "$(G_C)=====>     SUCCESS$(RESET_C)"
CLSUC				=	echo "$(R_C)=====>     DONE$(RESET_C)"
NORMD				=	echo "$(G_C)=====>     DONE$(RESET_C)"

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
	$(ECHO) $(GCFIL) $(NAME)
	$(CMPLO) $(NAME) $(OBJ) $(LIB) `sdl2-config --cflags --libs`
	$(GCSUC)
	echo "---\nCFLAGS - =$(B_C) $(CFLAGS)$(RESET_C)\n---"
	cp $(NAME) $(B_PATH)$(NAME)

$(OBJ): $(O_PATH)%.o: $(S_PATH)%.cpp $(HDR)
	$(CMPLC) $< -o $@ `sdl2-config --cflags --libs`
	$(ECHO) $(GCFIL) $<

$(PATHS):
	$(MKDIR) $(PATHS)
	$(foreach var,$(PATHS), $(ECHO) $(MKSHW) $(var);)

clean:
	for i in $(OBJ); do $(RM_RF) $$i; $(ECHO) $(RMSHW) $$i; done
	$(CLSUC)

fclean:
	clear ; for i in $(OBJ); do $(RM_RF) $$i; $(ECHO) $(RMSHW) $$i; done
	for i in $(PATHS); do $(RM_RF) $$i; $(ECHO) $(RMSHW) $$i; done
	$(RM_RF) $(NAME)
	$(ECHO) $(RMSHW) $(NAME)
	$(CLSUC)

help:
	echo "Makefile for gbmu"
	echo "usage : make [VERBOSE=1] [DEBUG=g|fsanitize|hard|dev] [all|clean|fclean|re|help]"

re:
	$(MAKE) --no-print-directory fclean all

ifndef VERBOSE
.SILENT:
endif

endif