/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 22:44:23 by lmariott          #+#    #+#             */
/*   Updated: 2022/12/24 04:27:43 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Loop.hpp"
#include "../includes/Cpu.hpp"
#include "../includes/Screen.hpp"

#include "../includes/Ppu.hpp"

#include "../includes/Debugger.hpp"
//#include "../includes/define.hpp"

#include <chrono>
#include <thread>
#include <iostream>

#define DELAY_TIME (1000.0f/60.0f)
bool Loop::loop()
{
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

	// TODO unsure about updateScreen ? Do we update everytime ?
	bool updateScreen = true;
	int clockDiff = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> current, previous, t1,t2;
	previous = std::chrono::high_resolution_clock::now();

	while (!Gameboy::quit)
	{
        current = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>> (current - previous);
		previous = current;
		/* Render clear */
        t1 = high_resolution_clock::now();
        updateScreen = Debugger::start(clockDiff, updateScreen);
		/* Manage events */
		Gameboy::pollEvent();
		/* Render present */
        t2 = high_resolution_clock::now();
		Gameboy::pollEvent();

        /* Getting number of milliseconds as a double. */
        duration<double, std::milli> ms_double = t2 - t1;
        std::cout << ms_double.count() << "ms\n";

		/* Sleep : TODO calculate compute time to have a frame rate ~60fps*/
        if (elapsed.count() < DELAY_TIME) {
            std::cout <<"sleep for "<< std::float_t() <<  DELAY_TIME - elapsed.count() << std::endl;
			std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(DELAY_TIME - elapsed.count()));
		} else{
			std::cout <<"overpassed by "<< std::float_t() <<  elapsed.count() - DELAY_TIME << std::endl;
		}
	}
	return (true);
}
