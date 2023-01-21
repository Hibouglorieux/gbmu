#include "Noise.hpp"


void Noise::channel_4_tick() {

    

    if (to_trigger && !trigger) {
        std::cout << "Channel 4 triggered\n";

        to_trigger = false;
        trigger = true;
    }

    // TODO implement sweep/envelope
}

Noise::Noise(int chan)
: channel(chan), trigger(false), to_trigger(false)
{
    std::cout << "Noise channel " << chan << " was created\n";
    if (chan != 4)
        throw "Wrong channel was specified for Noise channel";
}

Noise::~Noise()
{
}