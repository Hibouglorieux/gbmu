#include "APU.hpp"

unsigned char APU::triggers[4] = {0,0,0,0};
int APU::ticks = 0;
Waveform APU::channel3 = Waveform(3);
SquareWave APU::channel1 = SquareWave(1);
SquareWave APU::channel2 = SquareWave(2);

SDL_AudioSpec APU::obtainedSpec;
SDL_AudioSpec APU::desiredSpec;


int old_1 = 0;
int old_2 = 0;
int old_3 = 0;
int old_4 = 0;

void APU::init() {
    SDL_Init(SDL_INIT_AUDIO);
    std::cout << "INIT AUDIO\n";

    desiredSpec.freq = SAMPLING_RATE;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = 1;
    desiredSpec.samples = 2048;
    desiredSpec.callback = APU::sound_callback;
    desiredSpec.userdata = NULL;

    // you might want to look for errors here
    SDL_OpenAudio(&desiredSpec, &obtainedSpec);

    // start play audio
    SDL_PauseAudio(0);
}

void APU::tick(int n) {
    ticks += n;
    if (ticks >= 190) {
        channel2.tick(1);
        // channel1.tick(1);
        // checkTrigger(2);
        // checkTrigger(3);
        // checkTrigger(4);
    }
    ticks %= 190;
}

void APU::checkTrigger(int channel) {
    int currentValue = 0;

    switch (channel)
    {
    case 1:
        currentValue = BIT(mem[NR14], 7);
        break;
    case 2:
        currentValue = BIT(mem[NR24], 7);
        break;
    case 3:
        currentValue = BIT(mem[NR34], 7);
        break;
    case 4:
        currentValue = BIT(mem[NR44], 7);
        break;
    
    default:
        throw "Checking trigger on wrong audio channel";
    }

    if (currentValue && !triggers[channel - 1])
        trigger(channel);
    triggers[channel - 1] = currentValue;
}

void APU::trigger(int channel) {
    std::cout << "Triggering channel " << channel << "\n";
    switch (channel)
    {
    case 1:
        triggerChannel1();
        break;
    case 2:
        triggerChannel2();
        break;
    case 3:
        triggerChannel3();
        break;
    case 4:
        triggerChannel4();
        break;
    default:
        throw "Triggering wrong audio channel";
    }
}

void APU::triggerChannel1() {
    
    
}

void APU::triggerChannel2() {

}

void APU::triggerChannel3() {

}

void APU::triggerChannel4() {
    
}

APU::APU(/* args */)
{
}

APU::~APU()
{
}

void APU::sound_callback(void *arg, Uint8 *stream, int length) {

    // TODO verify for ptr[i] overflow
    uint16_t *ptr = (uint16_t *)stream;
    std::cout << "Sound callback\n";



    for (int i = 0; i < length/2; i++) {

        {
            // Channel 1
            int freq = 1048576/(2048 - channel1.waveLength);
            int samples_per_step = SAMPLING_RATE / freq;

            float val = (MAX_VOLUME * channel1.initialVolume)/0xF;
            if (channel1.trigger) {


                ptr[i] = BIT(channel1.wave, channel1.step) * (int)val * 0;
                channel1.iterations++;
                if (channel1.iterations >= samples_per_step) {
                    channel1.step = (channel1.step + 1) % 8;
                    channel1.iterations = 0;
                }
            }
            else 
                ptr[i] = 0;
        }

        {
            // Channel 2
            int freq = 1048576/(2048 - channel2.waveLength);
            int samples_per_step = SAMPLING_RATE / freq;

            float val = (MAX_VOLUME * channel2.initialVolume)/0xF;
            if (channel2.trigger) {


                ptr[i] += BIT(channel2.wave, channel2.step) * (int)val;
                channel2.iterations++;
                if (channel2.iterations >= samples_per_step) {
                    channel2.step = (channel2.step + 1) % 8;
                    channel2.iterations = 0;
                }
            }
            else 
                ptr[i] += 0;
        }

        {
            // Channel 3
            int freq = 2097152/(2048 - channel3.wavelength);
            float volumeFactor = (channel3.volume * MAX_VOLUME)/4;
            if (!channel3.DACenable)
                ptr[i] += 0;
            else {
                ptr[i] += channel3.waveform[channel3.step] * (int)volumeFactor;
                channel3.step = (channel3.step + 1) % 16;
            }
        }
    }
    // std::cout << "\n";
}