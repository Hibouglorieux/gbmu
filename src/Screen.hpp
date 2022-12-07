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
	static bool drawPoint(int x, int y, int r, int g, int b);
	static void destroy();
	static void update();
	static SDL_Window* get();
private:
	static SDL_Window*	window;
	static SDL_Renderer*	renderer;
};

#endif
