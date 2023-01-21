#include "Waveform.hpp"

void Waveform::tick(int n) {
    // TODO ?? Implement more than 1 tick (not necessary I think)
    (void)n;

    switch (channel)
    {
    case 3:
        channel_3_tick();
        break;
    default:
        throw "Trying to tick wrong Waveform channel";
    }
}

void Waveform::channel_3_tick() {

    DACenable = BIT(mem[NR30], 7);
    length_timer = mem[NR31];
    volume = (mem[NR32] & 0b01100000);
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
        throw "Wrong volume specified for Waveform channel\n";
    }
    wavelength = mem[NR33] | ((mem[NR34] & 0b111) << 8);
    length_enable = BIT(mem[NR34], 6);

    memcpy(waveform, &mem[0xFF30], 16);

    if (to_trigger && !trigger) {
        std::cout << "Channel 3 triggered\n";

        to_trigger = false;
        trigger = true;
    }

    // TODO implement sweep/envelope
}


Waveform::Waveform(int chan)
: channel(chan), step(0), trigger(false), to_trigger(false), iterations(0)
{
    std::cout << "Waveform channel " << chan << " was created\n";
    if (chan != 3)
        throw "Wrong channel was specified for Waveform channel";
}

Waveform::~Waveform()
{
}
