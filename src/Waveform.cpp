#include "Waveform.hpp"

void Waveform::tick() {
    // if (trigger)
    //     return;

    DACenable = BIT(mem[NR30], 7);
    length_timer = mem[NR31];
    volume = (mem[NR32] & 0b01100000) >> 5;
    switch (volume)
    {
    case 0:
    case 2:
        break;
    case 1:
        volume = 4;
        break;
    case 3:
        volume = 1;
        break;
    
    default:
        std::cout << "Volume : " << std::dec << (int)volume <<"\n";
        throw "Wrong volume specified for Waveform channel\n";
    }
    wavelength = mem[NR33] | ((mem[NR34] & 0b111) << 8);
    length_enable = BIT(mem[NR34], 6);
    memcpy(waveform, &mem[0xFF30], 16);

    // TODO implement sweep/envelope
}

void Waveform::triggerChannel() {
    std::cout << "Channel 3 triggered\n";
    to_trigger = false;
    trigger = true;
    current_length_timer = length_timer;
}


Waveform::Waveform(int chan)
: channel(chan), step(0), volume(0), trigger(false), to_trigger(false), iterations(0), length_count(0)
{
    std::cout << "Waveform channel " << chan << " was created\n";
    if (chan != 3)
        throw "Wrong channel was specified for Waveform channel";
}

Waveform::~Waveform()
{
}
