#include "../source/dsp/median_filter.hpp"

struct white_noise
{
    /* pd style white noise generator */
    white_noise(void)
    {
        static int init;
        init = (init * 435898247 + 382842987)^init; /* this seems to randomize the seed well enough */
        val = init;
    }

    float operator()(void)
    {
        val = val * 435898247 + 382842987;
        return ((float)((val & 0x7fffffff) - 0x40000000)) *
            (float)(1.0 / 0x40000000);
    }

    int val;
};

int main()
{
    nova::fractional_median_filter<float, 100> filter_;

    filter_.resize(45);

    white_noise noise;

    for (int i = 0; i != 10000000; ++i)
    {
        float smpl = noise();
        filter_.step(smpl);
    }
}
