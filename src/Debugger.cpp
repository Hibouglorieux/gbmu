
#include "../includes/Debugger.hpp"
#include <SDL2/SDL.h>

static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
bool Debugger::show_BG = false;
bool Debugger::show_Vram = false;
bool Debugger::show_hexdump = false;
bool Debugger::show_registers = false;

void hexdump_debugger() {
	{
		ImGui::Begin("Memory Hexdump:");

		// Display contents in a scrolling region
		ImGui::BeginChild("Scrolling");

		int perLine = 16;
		int i;
		unsigned char buff[perLine+1];
		int len = 0xffff+1;

		for (i = 0; i < len; i++) {

			if ((i % perLine) == 0) {

				if (i != 0) {
					ImGui::SameLine();
					ImGui::Text("  %s", buff);
					ImGui::NewLine();
				}
				// Output the offset of current line.
				ImGui::SameLine();
				ImGui::Text("  0x%04x ", i);
			}

			// Now the hex code for the specific character.
			ImGui::SameLine();
			ImGui::Text(" %02X", (int)mem[i]);

			// And buffer a printable ASCII character for later.
			if (std::isprint(mem[i]))
				buff[i % perLine] = mem[i];
			else
				buff[i % perLine] = '.';
			buff[(i % perLine) + 1] = '\0';
		}

		// Pad out last line if not exactly perLine characters.
		while ((i % perLine) != 0) {
			ImGui::SameLine();
			ImGui::Text("   ");
			i++;
		}

		// And print the final ASCII buffer.
		ImGui::SameLine();
		ImGui::Text("  %s", buff);
		ImGui::EndChild();
		ImGui::End();
	}
}

#include <chrono>
#include <iostream>
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;
std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;

void register_debugger() {
    {

        ImGui::Begin("Registers:");
        ImGui::Columns(2, "registers", true);
        ImGui::Separator();
        ImGui::Text("8 bits Register");
        ImGui::NewLine();

        ImGui::Text("A = [0x%02x]   F = [0x%02x]", Cpu::A, Cpu::F);
        ImGui::NextColumn();
        ImGui::Text("16 bits Register");
        ImGui::NewLine();
        ImGui::Separator();
        ImGui::Text("AF = [0x%04X]", Cpu::AF);
        ImGui::Separator();
        ImGui::NextColumn();

        ImGui::Text("B = [0x%02x]   C = [0x%02x]", Cpu::B, Cpu::C);
        ImGui::NextColumn();
        ImGui::Text("BC = [0x%04X]", Cpu::BC);
        ImGui::Separator();
        ImGui::NextColumn();

        ImGui::Text("D = [0x%02x]   E = [0x%02x]", Cpu::D, Cpu::E);
        ImGui::NextColumn();
        ImGui::Text("DE = [0x%04X]", Cpu::DE);
        ImGui::Separator();
        ImGui::NextColumn();

        ImGui::Text("H = [0x%02x]   L = [0x%02x]", Cpu::H, Cpu::L);
        ImGui::NextColumn();
        ImGui::Text("HL = [0x%04X]", Cpu::HL);
        ImGui::Separator();
        ImGui::NextColumn();

        ImGui::Text("       PC = [0x%04x]", Cpu::PC);
        ImGui::NextColumn();
        ImGui::Text("SP = [0x%04X]", Cpu::SP);
        ImGui::NewLine();

        ImGui::NextColumn();
        ImGui::Text("Flags:");
        ImGui::Separator();
        ImGui::Columns(4, "flags");
        ImGui::Text("Zero");
        ImGui::Text(Cpu::getZeroFlag() ? "1" : "0"); ImGui::NextColumn();
        ImGui::Text("Add/Sub");
        ImGui::Text(Cpu::getSubtractFlag() ? "1" : "0"); ImGui::NextColumn();
        ImGui::Text("Half carry");
        ImGui::Text(Cpu::getHalfCarryFlag() ? "1" : "0"); ImGui::NextColumn();
        ImGui::Text("Carry");
        ImGui::Text(Cpu::getCarryFlag() ? "1" : "0"); ImGui::NextColumn();
        ImGui::Columns(1);
        ImGui::Separator();
        t2 = high_resolution_clock::now();
        /* Getting number of milliseconds as a double. */
        duration<double, std::milli> ms_double = t2 - t1;
        ImGui::Text("%f ms\n", ms_double.count());
        ImGui::End();
    }
}



int Debugger::start(int clockDiff, bool updateScreen) {

    t1 = high_resolution_clock::now();
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    SDL_RenderClear(Screen::DBG_rend);

    {
        ImGui::Begin("PPU");
        if (ImGui::Button(show_BG ? "Hide BG" : "Show BG")) {
		    show_BG = !show_BG;
	    }
        ImGui::SameLine();
        if (ImGui::Button(show_Vram ? "show_Vram" : "show_Vram")) {
		    show_Vram = !show_Vram;
	    }
        ImGui::SameLine();
        if (ImGui::Button(show_hexdump ? "show_hexdump" : "show_hexdump")) {
		    show_hexdump = !show_hexdump;
	    }
        ImGui::SameLine();
        if (ImGui::Button(show_registers ? "show_registers" : "show_registers")) {
		    show_registers = !show_registers;
	    }
        ImGui::NewLine();
        Screen::drawPpu(clockDiff, updateScreen);
        ImGui::End();
    }

    if (show_hexdump)
        hexdump_debugger();

    if (show_Vram) {
        {
            ImGui::Begin("VRAM");
            Screen::drawVRam();
            ImGui::End();
        }
    }

    if (show_BG)
    {
        {
            ImGui::Begin("BG");
            Screen::drawBG();
            ImGui::End();
        }
    }

    if (show_registers) {
        register_debugger();
    }
    // SDL_Delay(3000);
    ImGui::Render();
    SDL_SetRenderDrawColor(Screen::DBG_rend, clear_color.x * 255, clear_color.y * 255, clear_color.z * 255, clear_color.w * 255);
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(Screen::DBG_rend);
	return updateScreen;
}
