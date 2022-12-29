/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 22:44:23 by lmariott          #+#    #+#             */
/*   Updated: 2022/12/26 15:22:37 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Loop.hpp"
#include "Ppu.hpp"
#include "Debugger.hpp"
#include <chrono>
#include <thread>

bool Loop::showVram = false;
bool Loop::showBG = false;
bool Loop::showHexdump = false;
bool Loop::showRegisters = true;

bool Loop::loop()
{
    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	const std::chrono::microseconds frametime(1'000'000 / 60);
	// TODO unsure about updateScreen ? Do we update everytime ?
	int clockDiff = 0;

	while (!Gameboy::quit)
	{
		auto beginFrameTime = std::chrono::system_clock::now();

	    Screen::NewframeTexture();

        {
            ImGui::Begin("PPU");
            if (ImGui::Button(showBG ? "Hide BG" : "Show BG")) {
                showBG = !showBG;
            }
            ImGui::SameLine();
            if (ImGui::Button(showVram ? "Hide Vram" : "Show Vram")) {
                showVram = !showVram;
            }
            ImGui::SameLine();
            if (ImGui::Button(showHexdump ? "Hide hexdump" : "Show hexdump")) {
                showHexdump = !showHexdump;
            }
            ImGui::SameLine();
            if (ImGui::Button(showRegisters ? "Show registers" : "Hide registers")) {
                showRegisters = !showRegisters;
            }
            ImGui::NewLine();
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            Screen::drawPpu(&clockDiff);
            Screen::TexturetoImage(Screen::texture);
            ImGui::End();
        }

        if (showVram) {
            {
                ImGui::Begin("Vram");
                Screen::drawVRam();
                Screen::TexturetoImage(Screen::VRamTexture);
                ImGui::End();
            }
        }

        if (showBG) {
            {
                ImGui::Begin("BackGround");
                if (ImGui::Button(DBG::bBGMap ? "Draw BG" : "Draw Window")) {
                    DBG::bBGMap = !DBG::bBGMap;
                }
                Screen::drawBG();
                Screen::TexturetoImage(Screen::BGTexture);
                ImGui::End();
            }
        }

        if (showRegisters) {
            DBG::registers();
        }

        if (showHexdump) {
            DBG::hexdump();
        }

        Ppu::resetWindowCounter();

		Gameboy::pollEvent();

		std::chrono::microseconds timeTakenForFrame = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - beginFrameTime);

		/* Sleep : TODO calculate compute time to have a frame rate ~60fps*/
		if (timeTakenForFrame.count() < frametime.count())
		{
			//std::cout << "sleeping for: " << std::dec << (frametime - timeTakenForFrame).count() << std::hex << " microseconds" << std::endl;
			std::this_thread::sleep_for(frametime - timeTakenForFrame);
		}
		else
		{
			//std::cout << "no need for sleep because frame took: " << std::dec << (timeTakenForFrame).count() << std::hex << " microseconds" << std::endl;
		}

        Screen::clear(clear_color);
	}
	return (true);
}
