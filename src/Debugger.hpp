#ifndef GBMU_DEBUGGER_H
#define GBMU_DEBUGGER_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"
#include <cstdio>
#include <SDL2/SDL.h>
#include "imgui/imconfig.h"
#include "Cpu.hpp"

enum class DebuggerState : int {
    PAUSED,
    RUNNING,
	ONCE
};

class DBG {
public:
    static DebuggerState state;
    static float speed;
    static bool bBGMap;
    static void registers();
    static void hexdump();

};


#endif //GBMU_DEBUGGER_H
