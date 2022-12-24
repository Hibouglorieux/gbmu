/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Screen.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <lmariott@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 20:46:17 by lmariott          #+#    #+#             */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCREEN_CLASS_H
# define SCREEN_CLASS_H

#include <SDL2/SDL.h>

class Screen {
public:
	static void handleEvent(SDL_Event *ev);
	static bool create();
	static bool drawPoint(int x, int y, int color, SDL_Renderer* renderer, int pixelScale = 4);
	static void destroy();
	static void update();
	static void clear();

	static void drawBG();
	static bool drawPpu(int, bool);
	static void drawVRam();
	static void display_tile(unsigned short location, unsigned short tileNum, int x, int y);

	static SDL_Window* get();

    static SDL_Window*      DBG_win;
    static SDL_Renderer*	DBG_rend;
    static SDL_Texture*     VRam_texture;
    static SDL_Texture*     BG_texture;
    static SDL_Texture*     Ppu_texture;


    //   	static SDL_Window*		window;
//	static SDL_Renderer*	renderer;
private:
//
//	static SDL_Window*		vRamWindow;
//	static SDL_Renderer*	vRamRenderer;
//	static SDL_Window*		backgroundWindow;
//	static SDL_Renderer*	backgroundRenderer;
};

#endif
