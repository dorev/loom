#pragma once

namespace Loom
{
    class Format
    {
    public:
        Format(unsigned channelCount, unsigned samplingRate)
            : channelCount(channelCount)
            , samplingRate(samplingRate)
        {}

        unsigned channelCount;
        unsigned samplingRate;

        bool operator==(const Format& other) const
        {
            return channelCount == other.channelCount && samplingRate == other.samplingRate;
        }
    };
}
