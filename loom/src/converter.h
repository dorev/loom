#pragma once

#include <cmath>
#include <array>

#include "error.h"

namespace Loom
{
    template <size_t N>
    struct FIRCoefficients
    {
        unsigned upFactor;
        unsigned downFactor;
        std::array<float, N> taps;
    };

    class Buffer;

    class Converter
    {
    public:
        static constexpr FIRCoefficients<32> FIR44100To48000 =
        {
            160,
            147,
            {
                -0.001f, -0.002f, -0.004f, -0.006f, -0.007f, -0.006f, -0.002f,  0.005f,
                0.014f,  0.025f,  0.035f,  0.042f,  0.044f,  0.039f,  0.025f,  0.001f,
                -0.032f, -0.071f, -0.113f, -0.152f, -0.184f, -0.201f, -0.198f, -0.170f,
                -0.118f, -0.043f,  0.048f,  0.152f,  0.262f,  0.371f,  0.473f,  0.562f
            }
        };

        static constexpr FIRCoefficients<32> FIR48000To44100 =
        {
            147,
            160,
            {
                -0.0005f, -0.0015f, -0.0035f, -0.0055f, -0.0065f, -0.0055f, -0.0020f,  0.0040f,
                0.0110f,  0.0200f,  0.0300f,  0.0370f,  0.0410f,  0.0370f,  0.0250f,  0.0005f,
                -0.0290f, -0.0650f, -0.1030f, -0.1400f, -0.1700f, -0.1850f, -0.1800f, -0.1550f,
                -0.1100f, -0.0400f,  0.0450f,  0.1400f,  0.2400f,  0.3400f,  0.4300f,  0.5100f
            }
        };
        static Error MonoToStereo(const Buffer* source, Buffer* destination, unsigned frameCount);
        static Error StereoToMono(const Buffer* source, Buffer* destination, unsigned frameCount);
        static Error Resample(Buffer* source, Buffer* destination, unsigned frameCount);

    private:
        template <size_t N>
        static Error ResampleFIR(const Buffer* source, Buffer* destination, unsigned frameCount, const FIRCoefficients<N>& coefficients);

        // Resampling with Windowed Sinc Interpolation
        // sincWindow: The number of neighboring samples to consider on either side for interpolation.
        //             Larger values improve quality but increase computation time.
        //             Typically, values between 8 and 32 are a good balance.
        // sincWidth:  Controls the effective width of the sinc filter (bandwidth scaling factor).
        //             Smaller values reduce aliasing but may impact transients and clarity.
        //             Typical values are between 2.0 and 6.0.
        static Error ResampleSincIntepolation(const Buffer* source, Buffer* destination, unsigned frameCount, unsigned sincWindow = 16, float sincWidth = 4.0f);

        static float Sinc(float x);
        static float HannWindow(float x);
    };
}
