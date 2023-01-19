/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Debugger.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/08 19:04:02 by lmariott          #+#    #+#             */
/*   Updated: 2023/01/12 02:25:27 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GBMU_DEBUGGER_H
#define GBMU_DEBUGGER_H

/*
** Debugger implementation
** Used to show some debug information in UserInterface
** Also used for controlling Gameboy for break/step
*/

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
	ONCE,
	ONCE_FRAME,
	ONCE_LINE
};

class Debugger {
public:
    static DebuggerState state;
    static int fps;
    static void registers();
    static void Sprites();
    static void hexdump();
	static unsigned int stopAtFrame;
};


#endif //GBMU_DEBUGGER_H
