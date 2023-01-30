/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserInterface.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmariott <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/06 22:44:23 by lmariott          #+#    #+#             */
/*   Updated: 2023/01/30 07:55:12 by lmariott         ###   ########.fr       */
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
bool UserInterface::bIsError = false;
bool UserInterface::bIsFatalError = false;
int UserInterface::volume = 0;
SDL_Window*	UserInterface::uiWindow = nullptr;
SDL_Renderer*	UserInterface::uiRenderer = nullptr;
std::string	UserInterface::romFolderPath = "";
std::string	UserInterface::errMsg = "";

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
	// Debugger::destroyTexture();
	SDL_DestroyRenderer(uiRenderer);
	SDL_DestroyWindow(uiWindow);
	SDL_QuitSubSystem(SDL_INIT_EVERYTHING);
	SDL_Quit();
	SDL_TLSCleanup();
}

bool UserInterface::create()
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

	// if (!Debugger::createTexture(bIsCGB, uiRenderer))
	// 	return false;

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

	char *home;
	if (!(home = getenv("HOME")))
	{
		std::cerr <<"Error : env $HOME missing"<< std::endl;
		return false;
	}
	romFolderPath = home;

	return (true);
}

void	UserInterface::newFrame()
{
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	SDL_RenderClear(uiRenderer);
	if (Gameboy::bIsInit) {
		Screen::lockTexture();
		Debugger::lockTexture();
	}
}

void	UserInterface::clear(ImVec4 vec4)
{
	ImGui::Render();
	SDL_SetRenderDrawColor(uiRenderer, vec4.x * 255, vec4.y * 255, vec4.z * 255, vec4.w * 255);
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(UserInterface::uiRenderer);
}

void UserInterface::showGameboyWindow()
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
	if (Gameboy::bIsCGB) {
		ImGui::SameLine();
		if (ImGui::Button(showPalettes ? "Hide palettes" : "Show palettes")) {
			showPalettes = !showPalettes;
		}
	}
	if (ImGui::Button("Reset")) {
		Gameboy::clear();
		ImGui::End();
		return ;
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Rom")) {
		Gameboy::clear();
		Gameboy::bIsPathValid = false;
		ImGui::End();
		return ;
	}
	ImGui::Text("Change this settings reset game : ");
	ImGui::SameLine();
	if (ImGui::Button("Autodetect CGB/DMG mode")) {
		Gameboy::clear();
		Gameboy::forceMode = false;
		ImGui::End();
		return ;
	}
	ImGui::SameLine();
	if (ImGui::Button("Force CGB mode")) {
		Gameboy::forceMode = true;
		Gameboy::forceCGB = true;
		Gameboy::clear();
		ImGui::End();
		return ;
	}
	ImGui::SameLine();
	if (ImGui::Button("Force DMG mode")) {
		Gameboy::forceMode = true;
		Gameboy::forceCGB = false;
		Gameboy::clear();
		ImGui::End();
		return ;
	}
	if (Gameboy::forceMode) {
		ImGui::Text("Forcing");
		ImGui::SameLine();
		ImGui::Text((Gameboy::forceCGB ? "CGB mode." : "DMG mode."));
	} else {
		ImGui::Text("Using autodetect DMG/CGB mode.");
	}
	ImGui::NewLine();
	ImGui::SetNextItemWidth(180);
	ImGui::SliderInt("FPS", &Debugger::fps, 1, 300);
	ImGui::SliderInt("Volume", &UserInterface::volume, 0, 100);
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
	ImGui::SameLine();
	if (ImGui::Button("Load State")) {
		Gameboy::loadSaveState();
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
	if (!UserInterface::bIsError && Debugger::stopAtFrame == Gameboy::frameNb)
		Debugger::state = DebuggerState::PAUSED;
	UserInterface::TexturetoImage(Screen::ppuTexture);
	ImGui::End();
}

void UserInterface::showSubWindows()
{
	if (!Gameboy::bIsInit)
		return ;
	if (showVram)
	{
		ImGui::Begin("Vram");
		Debugger::drawVRam(Gameboy::bIsCGB);
		UserInterface::TexturetoImage(Debugger::VRamTexture);
		ImGui::End();
	}
	if (showSprite)
	{
		ImGui::Begin("Sprite Map 8x8 only");
		Debugger::drawSprite();
		UserInterface::TexturetoImage(Debugger::SpriteTexture);
		Debugger::Sprites();
		ImGui::End();
	}
	if (showBG)
	{
		ImGui::Begin("Background/Window Map");
		ImGui::Text("Displaying map address: %04x", Debugger::mapAddr);
		ImGui::Text("BG map address:         %04x", (BIT(M_LCDC, 3) ? 0x9C00 : 0x9800));
		ImGui::Text("Window map address:     %04x", (BIT(M_LCDC, 6) ? 0x9C00 : 0x9800));
		if (ImGui::Button("Draw Window")) {
			Debugger::mapAddr = (BIT(M_LCDC, 6) ? 0x9C00 : 0x9800);
		}
		ImGui::SameLine();
		if (ImGui::Button("Draw Background")) {
			Debugger::mapAddr = (BIT(M_LCDC, 3) ? 0x9C00 : 0x9800);
		}
		ImGui::SameLine();
		if (ImGui::Button("Draw 0x9800")) {
			Debugger::mapAddr = 0x9800;
		}
		ImGui::SameLine();
		if (ImGui::Button("Draw 0x9C00")) {
			Debugger::mapAddr = 0x9C00;
		}
		ImGui::SameLine();
		if (ImGui::Button("Draw 0x8000")) {
			Debugger::mapAddr = 0x8000;
		}
		Debugger::drawBG(Debugger::mapAddr);
		UserInterface::TexturetoImage(Debugger::BGTexture);
		ImGui::End();
	}
	if (showPalettes)
	{
		if (Gameboy::bIsCGB) {
			ImGui::Begin("Palettes");
			Debugger::drawPalettes();
			ImGui::End();
		}
		else {
			showPalettes = false;
		}
	}

	if (showRegisters)
	{
		Debugger::registers();
	}

	if (showHexdump)
	{
		Debugger::hexdump();
	}
}

bool UserInterface::loop()
{	

	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	while (!Gameboy::quit)
	{
		std::chrono::microseconds frametime(1'000'000 / Debugger::fps);
		auto beginFrameTime = std::chrono::system_clock::now();

		UserInterface::newFrame();

		if (bIsError) {
			errorWindow();
		}
		else
		{
			if (!Gameboy::bIsInit) {
				if (Gameboy::bIsPathValid) {
					Gameboy::bIsInit = Gameboy::loadRom();
				}
			}
			if (!Gameboy::bIsPathValid) {
				// ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
				// ImGui::SetNextWindowSize(ImVec2(1900, 1000), ImGuiCond_FirstUseEver);
				// ImGui::Begin(UserInterface::romFolderPath.c_str());
				UserInterface::fileExplorer();
				//ImGui::End();
			}
			if (Gameboy::bIsInit && !UserInterface::bIsError) {
				UserInterface::showGameboyWindow();
				if (!UserInterface::bIsError) {
					UserInterface::showSubWindows();
				}
			}
		}

		Gameboy::pollEvent();

		std::chrono::microseconds timeTakenForFrame = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - beginFrameTime);

		/* Sleep : TODO calculate compute time to have a frame rate ~60fps*/
		if (timeTakenForFrame.count() < frametime.count())
		{
			std::this_thread::sleep_for(frametime - timeTakenForFrame);
//			printf("frametime - timeTakenForFrame = %d (timeTakenForFrame=%d) (frametime=%d)\n", (uint32_t)(frametime.count() - timeTakenForFrame.count()), (uint32_t)timeTakenForFrame.count(), (uint32_t)frametime.count());
		}
//		else
//			printf("frametime - timeTakenForFrame = %d (timeTakenForFrame=%d) (frametime=%d)\n", (uint32_t)(frametime.count() - timeTakenForFrame.count()), (uint32_t)timeTakenForFrame.count(), (uint32_t)frametime.count());
		
		UserInterface::clear(clear_color);
	}
	Gameboy::clear();
	destroy();
	return (true);
}
void	UserInterface::throwError(const char *msg, bool fatal)
{
	bIsError = true;
	bIsFatalError = fatal;
	errMsg = msg;
}

void	UserInterface::errorWindow()
{
	ImGui::Begin(bIsFatalError ? "FATAL ERROR" : "ERROR");
	ImGui::Text(errMsg.c_str());
	if (ImGui::Button("OK")) {
		if (bIsFatalError) {
			Gameboy::quit = true;
		}
		UserInterface::bIsError = false;
	}
	ImGui::End();
}

void	UserInterface::fileExplorer()
{
	// Yes, it's big
	char filename[8192] = {0};
	FILE *f = popen("zenity --file-selection --file-filter=\"Gameboy Rom | *.gb *.gbc\"", "r");
	fgets(filename, 8192, f);
	for (int i = 0 ; i < 8192 ; i++) {
		if (filename[i] == '\n') {
			filename[i] = 0;
			break ;
		}
	}
	filename[8191] = 0; // Ensure it last 0
	Gameboy::path = filename;
	Gameboy::bIsPathValid = true;

/*
** TODO old file explorer , to remove ?
**	DIR *dir;
**	struct dirent *entry;
**	struct stat info;
**
**	dir = opendir(UserInterface::romFolderPath.c_str());
**	while ((entry = readdir(dir)) != NULL)
**	{
**		std::string path = UserInterface::romFolderPath + "/" + std::string(entry->d_name);
**		stat(path.c_str(),&info);
**		if ((entry->d_name[0] != '.' || !strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) &&
**				(!strcmp(&entry->d_name[strlen(entry->d_name) - 4], ".gbc")
**				 || !strcmp(&entry->d_name[strlen(entry->d_name) - 3], ".gb")
**				 || S_ISDIR(info.st_mode)))
**		{
**
**			if (ImGui::Button(entry->d_name)) {
**				if (S_ISDIR(info.st_mode))
**				{
**					UserInterface::romFolderPath = path;
**				}
**				else
**				{
**					Gameboy::path = path;
**					Gameboy::bIsPathValid = true;
**				}
**			}
**		}
**	}
**	closedir(dir);
*/

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
	if (ev->type == SDL_DROPFILE)
	{
		Gameboy::clear();
		Gameboy::path = ev->drop.file;
		Gameboy::bIsPathValid = true;
		SDL_free(ev->drop.file);
	}

}
