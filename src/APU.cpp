#include "APU.hpp"

#define CHANNEL1 true
#define CHANNEL2 true
#define CHANNEL3 true
#define CHANNEL4 true

unsigned char APU::triggers[4] = {0,0,0,0};
int APU::ticks = 0;
int APU::masterVolume = 0;
SquareWave APU::channel1 = SquareWave(1);
SquareWave APU::channel2 = SquareWave(2);
Waveform APU::channel3 = Waveform(3);
Noise APU::channel4 = Noise(4);

SDL_AudioSpec APU::obtainedSpec;
SDL_AudioSpec APU::desiredSpec;

bool APU::addDecibel(unsigned short &ref, int val) {
    if (val <= 0)
        return false;
    int tmp = ref;
    float mixed = (val * masterVolume / 7);
    tmp += (int)mixed;
    // if (tmp > MAX_VOLUME)
    //     ref = MAX_VOLUME;
    // else
    ref = (unsigned short)tmp;
    return true;
}

void APU::init() {
    SDL_Init(SDL_INIT_AUDIO);
    std::cout << "INIT AUDIO\n";

    mem[NR10] = 0x80;
    mem[NR11] = 0xBF;
    mem[NR12] = 0xF3;
    mem[NR13] = 0xC1;
    mem[NR14] = 0xBF;

    mem[NR21] = 0x00;
    mem[NR22] = 0x00;
    mem[NR23] = 0x00;
    mem[NR24] = 0xB8;

    mem[NR30] = 0x7F;
    mem[NR31] = 0x00;
    mem[NR32] = 0x9F;
    mem[NR33] = 0x00;
    mem[NR34] = 0xB8;

    mem[NR41] = 0xC0;
    mem[NR42] = 0x00;
    mem[NR43] = 0x00;
    mem[NR44] = 0xBF;

    desiredSpec.freq = SAMPLING_RATE;
    desiredSpec.format = AUDIO_U16SYS;
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
        masterVolume = ((mem[0xFF24] & 0b111) + ((mem[0xFF24] & 0b1110000) >> 4)) / 2;
        if (!masterVolume)
            masterVolume = 1;

        channel2.tick();
        channel1.tick();
        channel3.tick();
        channel4.tick();
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
    // std::cout << "Sound callback\n";

    if (!BIT(mem[0xFF26], 7) || !BIT(mem[LCDC], 7)) {
        memset(stream, 0, length);
        return ;
    }

    for (int i = 0; i < length/2; i++) {
        ptr[i] = 0;

        if (CHANNEL1 && BIT(mem[0xFF26], 0))// && channel1.waveLength <= 0x7FF)
        {
            if (channel1.queue.size()) {
                channel1.popEntry(channel1.queue.front());
                channel1.queue.pop();

                channel1.volume = channel1.initialVolume;
                channel1.wavelengthSweepValue = 0;
                channel1.current_length_timer = channel1.length_timer;
                channel1.to_trigger = false;
                channel1.trigger = true;
                channel1.volumeReduction = 0;

                // std::cout << "Channel 1 playing :\n";
                // std::cout << "\tlength enable : " << channel1.lengthEnable << "\n";
                // std::cout << "\tlength timer : " << channel1.length_timer << "\n";
                // std::cout << "\tvolume : " << channel1.initialVolume << "\n";
                // std::cout << "\tvolume sweep direction : " << channel1.envelopeDirection << "\n";
                // std::cout << "\tvolume sweep pace : " << channel1.volumeSweepPace << "\n";
                // std::cout << "\twave length : " << channel1.waveLength << "\n";
                // std::cout << "\twave sweep direction : " << channel1.waveSweepDirection << "\n";
                // std::cout << "\twave sweep pace : " << channel1.waveSweepPace << "\n";
                // std::cout << "\twave sweep slope : " << channel1.waveSweepSlope << "\n";
            }
            if (channel1.trigger) {

                // Channel 1
                int freq = abs(1048576/(2048 - (int)((int)channel1.waveLength == 2048 ? 2047 : channel1.waveLength)));
                int samples_per_step = (SAMPLING_RATE / freq);

                float val = (float)(MAX_VOLUME * channel1.volume)/0x3;

                if (val)
                    addDecibel(ptr[i], (BIT(channel1.wave, channel1.step) * (int)val) - channel1.volumeReduction);

                channel1.iterations++;
                channel1.length_count++;

                if (channel1.iterations >= samples_per_step) {
                    channel1.step = (channel1.step + 1) % 8;
                    channel1.iterations = 0;
                }

                if (channel1.length_count >= SquareWave::samples_per_length) {
                    static int regulator = 0;
                    // Each 64 Hz
            
                    if (channel1.waveSweepPace && regulator < channel1.waveSweepSlope) {

                        // std::cout << "wavelength 1 : " << channel1.waveLength << " avec " << (channel1.waveLength / pow(2, channel1.waveSweepSlope)) << "\n"; 
                        if (channel1.waveSweepDirection)
                            channel1.changeWavelength(channel1.waveLength + (channel1.waveLength / pow(2, channel1.waveSweepSlope)));
                        else
                            channel1.changeWavelength(channel1.waveLength - (channel1.waveLength / pow(2, channel1.waveSweepSlope)));
                        // std::cout << "wavelength 2 : " << channel1.waveLength << "\n";
                    }

                    if (channel1.volumeSweepPace && regulator < channel1.volumeSweepPace) {
                        // std::cout << "volume 1-1 : " << channel1.volume << "\n";
                        if (channel1.envelopeDirection)
                            channel1.volumeReduction -= 500;
                        else if (!(regulator % 2))
                            channel1.volumeReduction += 500;
                        
                        if (channel1.volumeReduction > MAX_VOLUME)
                            channel1.trigger = false;
                        else if (channel1.volumeReduction < -MAX_VOLUME)
                            channel1.volumeReduction = -MAX_VOLUME;
                        // std::cout << "volume 1-2 : " << channel1.volume << "\n";
                    }

                    if (channel1.lengthEnable) {
                        channel1.current_length_timer--;
                        
                        if (channel1.current_length_timer <= 0) {
                            channel1.trigger = false;
                        }
                    }
                    channel1.length_count = 0;

                    regulator = (regulator == 6 ? 0 : regulator + 1);
                }
            }
        }


        if (CHANNEL2 && BIT(mem[0xFF26], 1))
        {
            if (channel2.queue.size()) {
                channel2.popEntry(channel2.queue.front());
                channel2.queue.pop();

                channel2.volume = channel2.initialVolume;
                channel2.wavelengthSweepValue = 0;
                channel2.current_length_timer = channel2.length_timer;
                channel2.to_trigger = false;
                channel2.trigger = true;
                channel2.volumeReduction = 0;
                

                // std::cout << "Channel 2 playing :\n";
                // std::cout << "\tlength enable : " << channel2.lengthEnable << "\n";
                // std::cout << "\tlength timer : " << channel2.length_timer << "\n";
                // std::cout << "\tvolume : " << channel2.initialVolume << "\n";
                // std::cout << "\tvolume sweep direction : " << channel2.envelopeDirection << "\n";
                // std::cout << "\tvolume sweep pace : " << channel2.volumeSweepPace << "\n";
                // std::cout << "\twave length : " << channel2.waveLength << "\n";
                // std::cout << "\twave sweep direction : " << channel2.waveSweepDirection << "\n";
                // std::cout << "\twave sweep pace : " << channel2.waveSweepPace << "\n";
                // std::cout << "\twave sweep slope : " << channel2.waveSweepSlope << "\n";
                
            }
            if (channel2.trigger) {
                // Channel 2
                int freq = abs(1048576/(2048 - (channel2.waveLength == 2048 ? 2047 : channel2.waveLength)));
                int samples_per_step = (SAMPLING_RATE / freq);

                float val = (float)(MAX_VOLUME * channel2.volume)/0x6;

                if (val)
                    addDecibel(ptr[i], (BIT(channel2.wave, channel2.step) * (int)val) - channel2.volumeReduction);
                    
                channel2.iterations++;
                channel2.length_count++;

                if (channel2.iterations >= samples_per_step) {
                    channel2.step = (channel2.step + 1) % 8;
                    channel2.iterations = 0;
                }

                if (channel2.length_count >= SquareWave::samples_per_length) {
                    // Each 64 Hz
                    static int regulator = 0;

                    if (channel2.volumeSweepPace && regulator < channel2.volumeSweepPace) {
                        if (channel2.envelopeDirection) {
                            channel2.volumeReduction -= 500;
                            std::cout << "volume reduction : " << std::dec << channel2.volumeReduction << "\n";
                        }
                        else if (!(regulator % 2)) {
                            channel2.volumeReduction += 500;
                            std::cout << "volume reduction : " << std::dec << channel2.volumeReduction << "\n";
                        }

                        if (channel2.volumeReduction > MAX_VOLUME)
                            channel2.trigger = false;
                        else if (channel2.volumeReduction < -MAX_VOLUME)
                            channel2.volumeReduction = -MAX_VOLUME;
                    }

                    if (channel2.lengthEnable) {
                        channel2.current_length_timer--;
                        
                        if (channel2.current_length_timer <= 0)
                            channel2.trigger = false;
                    }
                    channel2.length_count = 0;
                    regulator = (regulator == 6 ? 0 : regulator + 1);
                }
            }
        }

        

        if (CHANNEL3 && channel3.trigger)// && BIT(mem[0xFF26], 2))// && channel3.DACenable)
        {
            // Channel 3
            int freq = abs(2097152/(2048 - (channel3.wavelength == 2048 ? 2047 : channel3.wavelength)));
            int samples_per_step = (SAMPLING_RATE / freq);
            float volumeFactor = (channel3.volume * MAX_VOLUME)/(0xF * 4 * 7);

            static char tmp = 0;
            if (!(tmp % 2))
                addDecibel(ptr[i], (channel3.waveform[channel3.step/2] & 0x0F) * (int)volumeFactor);
            else
                addDecibel(ptr[i], (channel3.waveform[channel3.step/2] & 0xF0) * (int)volumeFactor);
            tmp++;
            
            channel3.iterations++;
            channel3.length_count++;

            if (channel3.iterations >= samples_per_step) {
                channel3.iterations = 0;
                channel3.step = (channel3.step + 1) % 32;
            }

            if (channel3.length_count >= Waveform::samples_per_length) {
                channel3.length_count = 0;

                if (channel3.length_enable) {
                    channel3.current_length_timer -= 2;
                    if (channel3.current_length_timer < 0)
                        channel3.current_length_timer = 0;

                    if (channel3.current_length_timer <= 0) {
                        channel3.trigger = false;
                    }
                }

            }
        }

        if (CHANNEL4 && channel4.trigger && BIT(mem[0xFF26], 3))
        {
            // Channel 4
            int freq = (262144 / (channel4.clockDivider * pow(2, channel4.clockShift)));

            int samples_per_step = (SAMPLING_RATE / freq);
            float volumeFactor = (channel4.initial_volume * MAX_VOLUME)/(0xF * 25);

            int val = ((channel4.sample) * (int)volumeFactor);
            if (val) {
                addDecibel(ptr[i], val - channel4.volumeReduction);

            }

            channel4.iterations++;
            channel4.length_count++;

            if (channel4.iterations >= samples_per_step) {
                channel4.iterations = 0;
                channel4.sample = rand() % 101;
            }

            if (channel4.length_count >= Noise::samples_per_length) {
                // Each 64 Hz
                static int regulator = 0;

                if (channel4.sweepPace && regulator < channel4.sweepPace) {
                    // std::cout << "volume 4-1 : " << channel4.initial_volume << "\n";
                    if (channel4.envelopeDirection) {
                        channel4.volumeReduction -= 300;
                    }
                    else {
                        channel4.volumeReduction += 300;
                    }

                    if (channel4.volumeReduction > MAX_VOLUME)
                        channel4.trigger = false;
                    else if (channel4.volumeReduction < -MAX_VOLUME)
                        channel4.volumeReduction = -MAX_VOLUME;
                    // std::cout << "volume 4-2 : " << channel4.initial_volume << "\n";
                }

                if (channel4.length_enable) {
                    channel4.current_length_timer -= 200;
                    
                    if (channel4.current_length_timer <= 0)
                        channel4.trigger = false;
                }
                channel4.length_count = 0;
                regulator = (regulator == 6 ? 0 : regulator + 1);
            }
        }
        unsigned short max = 0;
        float factor = 0;

        
    }
    // std::cout << "\n";
}