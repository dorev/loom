#include "buffer.h"
#include "converter.h"

namespace Loom
{
    Error Converter::MonoToStereo(const Buffer* source, Buffer* destination, unsigned frameCount)
    {
        for (unsigned i = 0; i < frameCount; ++i)
        {
            float sample = source->data[i];
            destination->data[2 * i] = sample * 0.707f;
            destination->data[2 * i + 1] = sample * 0.707f;
        }

        return OK;
    }

    Error Converter::StereoToMono(const Buffer* source, Buffer* destination, unsigned frameCount)
    {
        for (unsigned i = 0; i < frameCount; ++i)
        {
            float left = source->data[2 * i];
            float right = source->data[2 * i + 1];
            destination->data[i] = std::sqrt((left * left + right * right) / 2.0f);
        }

        return OK;
    }

    Error Converter::Resample(Buffer* source, Buffer* destination, unsigned frameCount)
    {
        if (source->format.frequency == 44100 && destination->format.frequency == 48000)
        {
            return ResampleFIR(source, destination, frameCount, FIR44100To48000);
        }
        else if (source->format.frequency == 48000 && destination->format.frequency == 44100)
        {
            return ResampleFIR(source, destination, frameCount, FIR48000To44100);
        }
        else
        {
            return ResampleSincIntepolation(source, destination, frameCount);
        }
    }

    template <size_t N>
    Error Converter::ResampleFIR(const Buffer* source, Buffer* destination, unsigned frameCount, const FIRCoefficients<N>& coefficients)
    {
        unsigned inputIndex = 0;
        unsigned outputIndex = 0;
        unsigned phase = 0; // Tracks fractional sample position
        unsigned channels = source->format.channelCount;

        for (unsigned i = 0; i < frameCount; ++i)
        {
            while (phase < coefficients.upFactor)
            {
                for (unsigned channel = 0; channel < channels; ++channel)
                {
                    float accumulator = 0.0f;

                    // Convolution
                    for (size_t j = 0; j < N; ++j)
                    {
                        int sampleIndex = inputIndex - static_cast<int>(j);
                        if (sampleIndex >= 0 && sampleIndex < static_cast<int>(source->data.size() / channels))
                        {
                            accumulator += source->data[sampleIndex * channels + channel] * coefficients.taps[j];
                        }
                    }

                    // Store interpolated value in destination buffer
                    destination->data[outputIndex * channels + channel] = accumulator;
                }

                // Advance phase
                phase += coefficients.downFactor;
                ++outputIndex;
            }

            // Reset phase and advance input index
            phase -= coefficients.upFactor;
            ++inputIndex;
        }

        return OK;
    }

    // Resampling with Windowed Sinc Interpolation
    // sincWindow: The number of neighboring samples to consider on either side for interpolation.
    //             Larger values improve quality but increase computation time.
    //             Typically, values between 8 and 32 are a good balance.
    // sincWidth:  Controls the effective width of the sinc filter (bandwidth scaling factor).
    //             Smaller values reduce aliasing but may impact transients and clarity.
    //             Typical values are between 2.0 and 6.0.
    Error Converter::ResampleSincIntepolation(const Buffer* source, Buffer* destination, unsigned frameCount, unsigned sincWindow, float sincWidth)
    {
        unsigned channels = source->format.channelCount;
        float ratio = static_cast<float>(source->format.frequency) / destination->format.frequency;

        for (unsigned i = 0; i < frameCount; ++i)
        {
            float sourceIndex = i * ratio;
            unsigned sourceIndexInt = static_cast<unsigned>(sourceIndex);

            for (unsigned channel = 0; channel < channels; ++channel)
            {
                float interpolatedSample = 0.0f;
                float normalization = 0.0f;

                int sincWindowInt = static_cast<int>(sincWindow);
                for (int j = -sincWindowInt; j <= sincWindowInt; ++j)
                {
                    int sampleIndex = sourceIndexInt + j;
                    if (sampleIndex >= 0 && sampleIndex < static_cast<int>(source->data.size() / channels))
                    {
                        float distance = sourceIndex - sampleIndex;
                        float sincValue = Sinc(distance) * HannWindow(distance / sincWidth);
                        interpolatedSample += source->data[sampleIndex * channels + channel] * sincValue;
                        normalization += sincValue;
                    }
                }

                if (normalization > 0.0f)
                    interpolatedSample /= normalization;

                destination->data[i * channels + channel] = interpolatedSample;
            }
        }

        return OK;
    }

    float Converter::Sinc(float x)
    {
        if (x == 0.0f)
            return 1.0f;
        x *= Pi;
        return std::sin(x) / x;
    }

    float Converter::HannWindow(float x)
    {
        if (std::abs(x) > 1.0f)
            return 0.0f;
        return 0.5f * (1.0f + std::cos(Pi * x));
    }
}
