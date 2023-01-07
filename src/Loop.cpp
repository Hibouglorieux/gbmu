/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Loop.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 22:44:23 by lmariott          #+#    #+#             */
/*   Updated: 2023/01/06 21:28:50 by nallani          ###   ########.fr       */
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
bool Loop::showRegisters = false;
bool Loop::showPalettes = false;

bool Loop::loop()
{	

    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	while (!Gameboy::quit)
	{
       	std::chrono::microseconds frametime(1'000'000 / DBG::fps);
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
            if (ImGui::Button(showRegisters ? "Hide registers" : "Show registers")) {
                showRegisters = !showRegisters;
            }
            ImGui::SameLine();
            if (ImGui::Button(showPalettes ? "Hide palettes" : "Show palettes")) {
                showPalettes = !showPalettes;
            }
            ImGui::NewLine();
			ImGui::SetNextItemWidth(180);
            ImGui::SliderInt("FPS", &DBG::fps, 1, 300);
			ImGui::SetNextItemWidth(180);
            ImGui::InputInt("frameNb Break", (int*)&DBG::stopAtFrame);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::SameLine();
            if (ImGui::Button(  DBG::state == DebuggerState::RUNNING ? "PAUSE" : "RUN")) {
                DBG::state = (DBG::state == DebuggerState::PAUSED) ? DebuggerState::RUNNING : DebuggerState::PAUSED;
            }
			ImGui::SameLine();
            if (ImGui::Button("Next step")) {
            	DBG::state = DebuggerState::ONCE;
            }
			ImGui::SameLine();
            if (ImGui::Button("Next frame")) {
            	DBG::state = DebuggerState::ONCE_FRAME;
            }
			ImGui::SameLine();
            if (ImGui::Button("Next line")) {
            	DBG::state = DebuggerState::ONCE_LINE;
            }
            if (ImGui::Button("Save State")) {
            	Gameboy::saveState();
            }
            if (DBG::state != DebuggerState::PAUSED) {
				Gameboy::Step step = Gameboy::Step::full;
				if (DBG::state == DebuggerState::ONCE)
					step = Gameboy::Step::oneInstruction;
				if (DBG::state == DebuggerState::ONCE_LINE)
					step = Gameboy::Step::oneLine;
            	Gameboy::execFrame(step);
            	if (DBG::state == DebuggerState::ONCE ||
            	    DBG::state == DebuggerState::ONCE_FRAME ||
					DBG::state == DebuggerState::ONCE_LINE) {
            	    DBG::state = DebuggerState::PAUSED;
            	}
            }
			if (DBG::stopAtFrame == Gameboy::frameNb)
				DBG::state = DebuggerState::PAUSED;
            Screen::TexturetoImage(Screen::texture);
            ImGui::End();
        }

        if (showVram) {
            {
                ImGui::Begin("Vram");
                Screen::drawVRam(Gameboy::bIsCGB);
                Screen::TexturetoImage(Screen::VRamTexture);
                ImGui::End();
            }
        }

        if (showBG) {
            {
				ImGui::Begin("Background/Window Map");
				ImGui::Text("Displaying map address: %04x", Screen::mapAddr);
				ImGui::Text("BG map address:         %04x", (BIT(M_LCDC, 3) ? 0x9C00 : 0x9800));
				ImGui::Text("Window map address:     %04x", (BIT(M_LCDC, 6) ? 0x9C00 : 0x9800));
                if (ImGui::Button("Draw Window")) {
					Screen::mapAddr = (BIT(M_LCDC, 6) ? 0x9C00 : 0x9800);
                }
				ImGui::SameLine();
                if (ImGui::Button("Draw Background")) {
					Screen::mapAddr = (BIT(M_LCDC, 3) ? 0x9C00 : 0x9800);
                }
				ImGui::SameLine();
                if (ImGui::Button("Draw 0x9800")) {
					Screen::mapAddr = 0x9800;
				}
				ImGui::SameLine();
                if (ImGui::Button("Draw 0x9C00")) {
					Screen::mapAddr = 0x9C00;
				}
                Screen::drawBG(Screen::mapAddr);
                Screen::TexturetoImage(Screen::BGTexture);
                ImGui::End();
            }
        }
		if (showPalettes)
		{
			ImGui::Begin("Palettes");
			Screen::drawPalettes();
			ImGui::End();
		}

        if (showRegisters) {
            DBG::registers();
        }

        if (showHexdump) {
            DBG::hexdump();
        }

	Gameboy::pollEvent();

	std::chrono::microseconds timeTakenForFrame = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - beginFrameTime);

	/* Sleep : TODO calculate compute time to have a frame rate ~60fps*/
	if (timeTakenForFrame.count() < frametime.count())
	{
			// std::cout << "sleeping for: " << std::dec << (frametime - timeTakenForFrame).count() << std::hex << " microseconds" << std::endl;
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
