#include "SquareWave.hpp"
#include "Gameboy.hpp"
#include "SoundRegisters.hpp"
#include <iostream>

void SquareWave::tick(int index) {

    switch (channel)
    {
    case 1:
        channel_1_tick(index);
        break;
    case 2:
        channel_2_tick(index);
        break;
    default:
        throw "Trying to tick wrong SquareWave channel";
    }
}

void SquareWave::channel_2_tick(int index) {

    length_timer = (registers[0].channel2.nr21 & 0b00111111);
    volumeSweepPace = (registers[0].channel2.nr22 & 0b00000111);
    initialVolume = (registers[0].channel2.nr22 & 0b11110000) >> 4;
    envelopeDirection = BIT(registers[0].channel2.nr22, 3);
    waveLength = ((registers[0].channel2.nr24 & 0b111) << 8) | registers[0].channel2.nr23;
    waveDuty = (registers[0].channel2.nr21 & 0b11000000) >> 6;
    lengthEnable = BIT(registers[0].channel2.nr24, 6);

    DACenable = (~(0b111) & registers[0].channel2.nr22) != 0;

    switch (waveDuty)
    {
    case 0:
        wave = 0b00000001;
        break;
    case 1:
        wave = 0b10000001;
        break;
    case 2:
        wave = 0b10000111;
        break;
    case 3:
        wave = 0b11100111;
        break;
    
    default:
        throw "Wrong waveDuty specified for SquareWave";
    }

    // lengthEnable = BIT(registers[0].channel2.nr24, 6);
    // waveLength = ((registers[0].channel2.nr24 & 0b111) << 8) | registers[index].channel2.nr23;

    // DACenable = (~(0b111) & registers[0].channel2.nr22) != 0;

    // // if (lengthEnable)
    //     // std::cout << "Channel 2 tick : " << waveLength << "\n";
    // // std::cout << "CHANNEL 2 TICK : " << lengthEnable << " et " << volumeSweepPace << " avec " << envelopeDirection << "\n";


    // switch (waveDuty)
    // {
    // case 0:
    //     wave = 0b00000001;
    //     break;
    // case 1:
    //     wave = 0b10000001;
    //     break;
    // case 2:
    //     wave = 0b10000111;
    //     break;
    // case 3:
    //     wave = 0b11100111;
    //     break;
    
    // default:
    //     throw "Wrong waveDuty specified for SquareWave";
    // }
}

void SquareWave::popEntry(entry val) {
    step = 0;

    current_length_timer = length_timer;
    volume = initialVolume;

    trigger = true;
    wavelengthSweepValue = 0;

    ticks = 0;
    iterations = 0;

    length_count = 0;
    volumeReduction = 0;

}

void SquareWave::clear() {
    step = 0;

    trigger = false;
    
    // if (channel == 1) {
    //     mem[NR10] = 0;
    //     mem[NR11] = 0;
    //     mem[NR12] = 0;
    //     mem[NR13] = 0;
    //     mem[NR14] = 0;
    // } else {
    //     mem[NR21] = 0;
    //     mem[NR22] = 0;
    //     mem[NR23] = 0;
    //     mem[NR24] = 0;
    // }

    queue = std::queue<entry>();
}

void SquareWave::triggerChannel() {
    // if (trigger) return;

    entry tmp;
    if (channel == 2) {
        // std::cout << "Channel " << channel << " triggered : \n";

        tmp.length_timer = (registers[0].channel2.nr21 & 0b00111111);
        tmp.volumeSweepPace = (registers[0].channel2.nr22 & 0b00000111);
        tmp.initialVolume = (registers[0].channel2.nr22 & 0b11110000) >> 4;
        tmp.envelopeDirection = BIT(registers[0].channel2.nr22, 3);
        tmp.waveLength = ((registers[0].channel2.nr24 & 0b111) << 8) | registers[0].channel2.nr23;
        tmp.waveDuty = (registers[0].channel2.nr21 & 0b11000000) >> 6;
        tmp.length_enable = BIT(registers[0].channel2.nr24, 6);
        
        // std::cout << std::hex << (int)mem[NR21] << " - " << (int)mem[NR22] << " - " << (int)mem[NR23] << " - " << (int)(mem[NR24] | 0b00111000) << "\n";
    }
    else {
        // std::cout << "Channel " << channel << " triggered : \n";
        tmp.length_timer = (registers[0].channel1.nr11 & 0b00111111);
        tmp.volumeSweepPace = (registers[0].channel1.nr12 & 0b00000111);
        tmp.initialVolume = (registers[0].channel1.nr12 & 0b11110000) >> 4;
        tmp.envelopeDirection = BIT(registers[0].channel1.nr12, 3);
        tmp.waveLength = ((registers[0].channel1.nr14 & 0b111) << 8) | registers[0].channel1.nr13;
        tmp.waveDuty = (registers[0].channel1.nr11 & 0b11000000) >> 6;
        tmp.length_enable = BIT(registers[0].channel1.nr14, 6);

        tmp.waveSweepDirection = BIT(registers[0].channel1.nr10, 3);
        tmp.waveSweepPace = (registers[0].channel1.nr10 & 0b01110000) >> 4;
        tmp.waveSweepSlope = (registers[0].channel1.nr10 & 0b00000111);
        
        // std::cout << std::hex << (int)mem[NR10] << " - " << (int)mem[NR11] << " - " << (int)mem[NR12] << " - " << (int)mem[NR13] << " - " << (int)mem[NR14] << "\n";

    }

    queue.push(tmp);
    // std::cout << "\tlength enable : " << tmp.length_enable << "\n";
    // std::cout << "\tlength timer : " << tmp.length_timer << "\n";
    // std::cout << "\tvolume : " << tmp.initialVolume << "\n";
    // std::cout << "\tvolume sweep direction : " << tmp.envelopeDirection << "\n";
    // std::cout << "\tvolume sweep pace : " << tmp.volumeSweepPace << "\n";
    // std::cout << "\twave length : " << tmp.waveLength << "\n";
    // std::cout << "\twave sweep direction : " << tmp.waveSweepDirection << "\n";
    // std::cout << "\twave sweep pace : " << tmp.waveSweepPace << "\n";
    // std::cout << "\twave sweep slope : " << tmp.waveSweepSlope << "\n";
}

void SquareWave::channel_1_tick(int index) {
    // if (trigger)
    //     return ;

    length_timer = (registers[0].channel1.nr11 & 0b00111111);
    volumeSweepPace = (registers[0].channel1.nr12 & 0b00000111);
    initialVolume = (registers[0].channel1.nr12 & 0b11110000) >> 4;
    envelopeDirection = BIT(registers[0].channel1.nr12, 3);
    waveLength = ((registers[0].channel1.nr14 & 0b111) << 8) | registers[0].channel1.nr13;
    waveDuty = (registers[0].channel1.nr11 & 0b11000000) >> 6;
    lengthEnable = BIT(registers[0].channel1.nr14, 6);

    waveSweepDirection = BIT(registers[0].channel1.nr10, 3);
    waveSweepPace = (registers[0].channel1.nr10 & 0b01110000) >> 4;
    waveSweepSlope = (registers[0].channel1.nr10 & 0b00000111);
    DACenable = (~(0b111) & registers[0].channel1.nr12) != 0;


    switch (waveDuty)
    {
    case 0:
        wave = 0b00000001;
        break;
    case 1:
        wave = 0b10000001;
        break;
    case 2:
        wave = 0b10000111;
        break;
    case 3:
        wave = 0b11100111;
        break;
    
    default:
        throw "Wrong waveDuty specified for SquareWave";
    }

    // lengthEnable = BIT(registers[0].channel1.nr14, 6);
    // waveLength = ((registers[0].channel1.nr14 & 0b111) << 8) | registers[0].channel1.nr13;
    // volume = (registers[0].channel1.nr12 & 0b11110000) >> 4;

    // waveSweepSlope = (registers[0].channel1.nr10 & 0b00000111);

    // DACenable = (~(0b111) & registers[0].channel1.nr12) != 0;
}

void SquareWave::changeWavelength(float val) {
    waveLength = val;
    if (channel == 1) {
        registers[0].channel1.nr13 = ((int)val & 0xFF);
        registers[0].channel1.nr14 &= ~0b111;
        registers[0].channel1.nr14 |= ((int)val >> 8) & 0b111;
    } else {
        registers[0].channel2.nr23 = ((int)val & 0xFF);
        registers[0].channel2.nr24 &= ~0b111;
        registers[0].channel2.nr24 |= ((int)val >> 8) & 0b111;
    }
}

SquareWave* SquareWave::loadSquareWave(int chan, std::vector<sound_registers> &ref)
{
	return new SquareWave(chan, ref);
}

SquareWave::SquareWave(int chan, std::vector<sound_registers> &regs)
: channel(chan), step(0), initialVolume(0), trigger(false), iterations(0), length_count(0), registers(regs)
{
    std::cout << "SquareWave channel " << chan << " was created\n";
    if (chan != 1 && chan != 2)
        throw "Wrong channel was specified for SquareWave channel";
    ticks = 0;
	waveDuty = 0;
	DACenable = 0;
}

SquareWave::~SquareWave()
{
}
