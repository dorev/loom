#pragma once

namespace Loom
{
    class Format
    {
    public:
        Format(unsigned channelCount = 0, unsigned samplingRate = 0)
            : channelCount(channelCount)
            , frequency(samplingRate)
        {}

        unsigned channelCount;
        unsigned frequency;

        bool operator==(const Format& other) const
        {
            return channelCount == other.channelCount && frequency == other.frequency;
        }
    };
}
