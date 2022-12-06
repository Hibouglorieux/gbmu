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
	Screen();
	~Screen();
	unsigned char& operator[](int i);
	static void inputWatcher();
	static void createWindow();
private:
	unsigned char* internalArray;
	SDL_Window* window;
};

#endif
