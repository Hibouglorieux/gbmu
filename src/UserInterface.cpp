/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserInterface.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 22:44:23 by lmariott          #+#    #+#             */
/*   Updated: 2023/01/06 21:28:50 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "UserInterface.hpp"
#include "Ppu.hpp"
#include "Debugger.hpp"
#include <chrono>
#include <thread>

bool UserInterface::showVram = false;
bool UserInterface::showBG = false;
bool UserInterface::showSprite = false;
bool UserInterface::showHexdump = false;
bool UserInterface::showRegisters = false;
bool UserInterface::showPalettes = false;
SDL_Window*	UserInterface::uiWindow = nullptr;
SDL_Renderer*	UserInterface::uiRenderer = nullptr;

void UserInterface::TexturetoImage(SDL_Texture * Texture)
{
	int width;
	int height;

    	SDL_SetRenderTarget(uiRenderer, Texture);
   	SDL_UnlockTexture(Texture);
	SDL_QueryTexture(Texture, nullptr, nullptr, &width, &height);
    	ImGui::Image((void*)(intptr_t)Texture, ImVec2(width, height));
//	SDL_RenderCopy(uiRenderer, Texture, NULL, NULL);
    	SDL_SetRenderTarget(uiRenderer, nullptr);
}

void UserInterface::destroy()
{
    	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	Screen::destroyTexture();
	SDL_DestroyRenderer(uiRenderer);
	SDL_DestroyWindow(uiWindow);
    	SDL_Quit();
}

bool UserInterface::create(bool bIsCGB)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		std::cerr <<"Error SDL_Init! "<< SDL_GetError() << std::endl;
		return false;
	}

	auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	uiWindow = SDL_CreateWindow("GBMU",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			1980,
			1024,
			window_flags);
	if (!uiWindow) {
		std::cerr <<"Error SDL_CreateWindow! "<< SDL_GetError() << std::endl;
		return (false);
	}


	uiRenderer = SDL_CreateRenderer(uiWindow, -1,  SDL_RENDERER_ACCELERATED); //SDL_RENDERER_PRESENTVSYNC
	if (!uiRenderer)
	{
		std::cerr <<"Error SDL_CreateRenderer : "<< SDL_GetError() << std::endl;
		return false;
	}

	// TODO LMA move it to a Screen::init
	if (!Screen::createTexture(bIsCGB, uiRenderer))
		return false;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(uiWindow, uiRenderer);
	ImGui_ImplSDLRenderer_Init(uiRenderer);
	SDL_RenderClear(uiRenderer);
	return (true);
}

void	UserInterface::newFrame()
{
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	SDL_RenderClear(uiRenderer);
	Screen::lockTexture();
}

void	UserInterface::clear(ImVec4 vec4)
{
    ImGui::Render();
    SDL_SetRenderDrawColor(uiRenderer, vec4.x * 255, vec4.y * 255, vec4.z * 255, vec4.w * 255);
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
    SDL_RenderPresent(UserInterface::uiRenderer);
}

bool UserInterface::loop()
{	

    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	while (!Gameboy::quit)
	{
       	std::chrono::microseconds frametime(1'000'000 / Debugger::fps);
		auto beginFrameTime = std::chrono::system_clock::now();

	    UserInterface::newFrame();

        {
            ImGui::Begin("PPU");
            if (ImGui::Button(showBG ? "Hide BG" : "Show BG")) {
                showBG = !showBG;
            }
            ImGui::SameLine();
            if (ImGui::Button(showSprite ? "Hide Sprites" : "Show Sprites")) {
                showSprite = !showSprite;
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
            ImGui::SliderInt("FPS", &Debugger::fps, 1, 300);
			ImGui::SetNextItemWidth(180);
            ImGui::InputInt("frameNb Break", (int*)&Debugger::stopAtFrame);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::SameLine();
            if (ImGui::Button(  Debugger::state == DebuggerState::RUNNING ? "PAUSE" : "RUN")) {
                Debugger::state = (Debugger::state == DebuggerState::PAUSED) ? DebuggerState::RUNNING : DebuggerState::PAUSED;
            }
			ImGui::SameLine();
            if (ImGui::Button("Next step")) {
            	Debugger::state = DebuggerState::ONCE;
            }
			ImGui::SameLine();
            if (ImGui::Button("Next frame")) {
            	Debugger::state = DebuggerState::ONCE_FRAME;
            }
			ImGui::SameLine();
            if (ImGui::Button("Next line")) {
            	Debugger::state = DebuggerState::ONCE_LINE;
            }
            if (ImGui::Button("Save State")) {
            	Gameboy::saveState();
            }
            if (Debugger::state != DebuggerState::PAUSED) {
				Gameboy::Step step = Gameboy::Step::full;
				if (Debugger::state == DebuggerState::ONCE)
					step = Gameboy::Step::oneInstruction;
				if (Debugger::state == DebuggerState::ONCE_LINE)
					step = Gameboy::Step::oneLine;
            	Gameboy::execFrame(step);
            	if (Debugger::state == DebuggerState::ONCE ||
            	    Debugger::state == DebuggerState::ONCE_FRAME ||
					Debugger::state == DebuggerState::ONCE_LINE) {
            	    Debugger::state = DebuggerState::PAUSED;
            	}
            }
			if (Debugger::stopAtFrame == Gameboy::frameNb)
				Debugger::state = DebuggerState::PAUSED;
            UserInterface::TexturetoImage(Screen::ppuTexture);
            ImGui::End();
        }

        if (showVram) {
            {
                ImGui::Begin("Vram");
                Screen::drawVRam(Gameboy::bIsCGB);
                UserInterface::TexturetoImage(Screen::VRamTexture);
                ImGui::End();
            }
        }

        if (showSprite) {
		ImGui::Begin("Sprite Map 8x8 only");
		Screen::drawSprite();
                UserInterface::TexturetoImage(Screen::SpriteTexture);
		Debugger::Sprites();
                ImGui::End();
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
		ImGui::SameLine();
                if (ImGui::Button("Draw 0x8000")) {
			Screen::mapAddr = 0x8000;
		}
                Screen::drawBG(Screen::mapAddr);
                UserInterface::TexturetoImage(Screen::BGTexture);
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
            Debugger::registers();
        }

        if (showHexdump) {
            Debugger::hexdump();
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
        UserInterface::clear(clear_color);
	}
	return (true);
}

void	UserInterface::handleEvent(SDL_Event *ev)
{
	ImGui_ImplSDL2_ProcessEvent(ev);
	if (ev->type == SDL_WINDOWEVENT) {
		switch (ev->window.event) {
			case SDL_WINDOWEVENT_CLOSE:
				if (ev->window.windowID
						== SDL_GetWindowID(uiWindow)) {
					Gameboy::quit = true;
					Gameboy::saveRam();
				}
				break;
		}
	}
	if (ev->type == SDL_KEYDOWN)
	{
		if (ev->key.keysym.sym == SDLK_ESCAPE) {
			Gameboy::quit = true;
			Gameboy::saveRam();
		}
	}
}
