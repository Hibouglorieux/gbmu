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

#define MAIN_SCREEN_SCALE 4
#define VRAM_SCREEN_SCALE 2
#define BG_SCREEN_SCALE 2


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

	static void		lockTexture();
	static void		destroyTexture();
	static bool		createTexture(bool, SDL_Renderer *);
	static void		drawBG(int mapAddr);
	static void		drawSprite(void);
	static void		drawVRam(bool bIsCGB);
	static void		drawPalettes();

	static int		mapAddr;
	static unsigned int stopAtFrame;

	static SDL_Texture*	BGTexture;
	static void*		BGPixels;
	static int		BGPitch;

	static SDL_Texture*	SpriteTexture;
	static void*		SpritePixels;
	static int		SpritePitch;

	static SDL_Texture*	VRamTexture;
	static void*		VramPixels;
	static int		VramPitch;

};


#endif //GBMU_DEBUGGER_H
