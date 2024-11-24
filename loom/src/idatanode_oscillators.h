#pragma once

#include "idatanode.h"
#include "format.h"
#include "buffer.h"

namespace Loom
{
    class SineSource : public IDataNode
    {
    public:
        SineSource(float frequency, float amplitude)
            : frequency(frequency), amplitude(amplitude), phase(0.0f)
        {
        }

    public:
        float frequency;
        float amplitude;

    protected:
        Error Read(Buffer* buffer, unsigned bufferFrameOffset, unsigned /*seekPosition*/, unsigned frameCount, unsigned& framesRead) override
        {
            Format format = buffer->format;
            for (unsigned i = 0; i < frameCount; ++i)
            {
                float sample = amplitude * sin(phase);

                for (unsigned channel = 0; channel < format.channelCount; ++channel)
                    buffer->data[(i + bufferFrameOffset) * format.channelCount + channel] = sample;

                phase += 2.0f * Pi * frequency / format.frequency;

                if (phase >= 2.0f * Pi)
                    phase -= 2.0f * Pi;
            }

            framesRead = frameCount;
            return OK;
        }

    private:
        float phase;
    };

    class TriangleSource : public IDataNode
    {
    public:
        TriangleSource(float frequency, float amplitude)
            : frequency(frequency)
            , amplitude(amplitude)
            , framesWritten(0)
            , sample(0.0f)
            , increment(true)
        {
        }

    public:
        float frequency;
        float amplitude;

    protected:
        Error Read(Buffer* buffer, unsigned bufferFrameOffset, unsigned /*seekPosition*/, unsigned frameCount, unsigned& framesRead) override
        {
            Format format = buffer->format;

            float frameAmplitudeStep = amplitude * 2.0f * frequency / format.frequency;

            for (unsigned i = 0; i < frameCount; ++i)
            {
                if (increment)
                {
                    sample += frameAmplitudeStep;
                    if (sample >= amplitude)
                    {
                        sample = amplitude;
                        increment = false;
                    }
                }
                else
                {
                    sample -= frameAmplitudeStep;
                    if (sample <= -amplitude)
                    {
                        sample = -amplitude;
                        increment = true;
                    }
                }

                for (unsigned channel = 0; channel < format.channelCount; ++channel)
                    buffer->data[(i + bufferFrameOffset) * format.channelCount + channel] = sample;
            }

            framesRead = frameCount;
            return OK;
        }

        bool increment;
        float sample;
        unsigned framesWritten;
    };

}
