//
// Created by lucky on 14/12/22.
//

#ifndef GBMU_DEBUGGER_H
#define GBMU_DEBUGGER_H

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include <cstdio>
#include <SDL2/SDL.h>
#include "imconfig.h"
#include "Cpu.hpp"

//enum class DebuggerState : int {
//    PAUSED,
//    RUNNING
//}

class DBG {
public:
//    DebuggerState state;

//    static int start(int, bool);
    static bool bBGMap;
    static void registers();
    static void hexdump();

};


#endif //GBMU_DEBUGGER_H
