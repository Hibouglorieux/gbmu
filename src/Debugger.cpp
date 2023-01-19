
#include "Debugger.hpp"
#include <SDL2/SDL.h>

DebuggerState Debugger::state = DebuggerState::RUNNING;
int Debugger::fps = 60;
unsigned int Debugger::stopAtFrame = 0;

void Debugger::hexdump() {
	{
	ImGui::Begin("Memory Hexdump:");

//        ImGui::Text("Number of Rom: %d\nCurrent Rombank for 1st slot: %d\nCurrent Rombank for 2nd slot:", mem.mbc->getRomBank(0));
//        ImGui::SameLine();
//        ImGui::Text("Number of Ram: %d\nCurrent Rambank: %d\n");
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

void Debugger::registers() {
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

        ImGui::Text("       LY = [0x%02X]", (int)M_LY);
        ImGui::Text("       PC = [0x%04X]", Cpu::PC);
        ImGui::Text("       opcode = [0x%02X] %02X %02X", (int)mem[Cpu::PC], (int)mem[Cpu::PC + 1], (int)mem[Cpu::PC + 2]);
        ImGui::NextColumn();
        ImGui::Text("SP = [0x%04X]", Cpu::SP);
        ImGui::Text("frameNumber : %u", Gameboy::frameNb);
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
        ImGui::Columns(1);
        ImGui::Separator();

        ImGui::Text("Interrupts:");
        ImGui::Checkbox("Interupts Master Enable (IME)",&Cpu::interrupts_master_enable);
    	unsigned char ie = M_EI;
    	unsigned char iflag = M_IF;
    	ImGui::Text("Interrupt Enable (0xFFFF): 0x%02X\nInterrupt Flag   (0xFF0F): 0x%02X",ie, iflag);
    	ImGui::Separator();
    	ImGui::Columns(6, "flags"); ImGui::NewLine();
        ImGui::Text("IE ="); ImGui::NextColumn();
        ImGui::Text("V-Blank");
    	ImGui::Text(BIT(ie, 0) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Text("LCD Status");
    	ImGui::Text(BIT(ie, 1) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Text("Timer");
    	ImGui::Text(BIT(ie, 2) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Text("Serial");
    	ImGui::Text(BIT(ie, 3) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Text("Joypad");
    	ImGui::Text(BIT(ie, 4) ? "1" : "0"); ImGui::NextColumn();
        ImGui::Text("IF ="); ImGui::NextColumn();
    	ImGui::Text(BIT(iflag, 0) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Text(BIT(iflag, 1) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Text(BIT(iflag, 2) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Text(BIT(iflag, 3) ? "1" : "0"); ImGui::NextColumn();
        ImGui::Text(BIT(iflag, 4) ? "1" : "0"); ImGui::NextColumn();
    	ImGui::Columns(1);
    	ImGui::Separator();
        ImGui::End();
    }
}

void Debugger::Sprites() {
	const int OAM_Addr = 0xFE00;

	for (int i = 0; i < MAX_SPRITES; i++) {
		struct OAM_entry *entry = (struct OAM_entry *)(&mem[OAM_Addr + i*4]);
		ImGui::Text("Sprite Index:%02d", i);
		ImGui::Text("tileIndex:%02x ", entry->tileIndex);
            	ImGui::SameLine();
		ImGui::Text("posY:%02x ", entry->posY);
            	ImGui::SameLine();
		ImGui::Text("posX:%02x ", entry->posX);
            	ImGui::SameLine();
		ImGui::Text("attributes:%02x", entry->attributes);
		ImGui::NewLine();
	}
}
