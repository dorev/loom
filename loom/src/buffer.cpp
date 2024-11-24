#include "buffer.h"
#include "converter.h"

namespace Loom
{

    Buffer::Buffer(unsigned frameCount, Format format)
        : format(format)
        , assigned()
    {
        Reset(frameCount);
    }

    void Buffer::Reset(unsigned frameCount)
    {
        if (!CanContainXFrames(frameCount))
            data.resize(frameCount * format.channelCount);

        std::fill(data.begin(), data.end(), 0.0f);
    }

    bool Buffer::CanContainXFrames(unsigned frameCount) const
    {
        return data.capacity() >= (frameCount * format.channelCount);
    }

    unsigned Buffer::FrameCapacity() const
    {
        return static_cast<unsigned>(data.capacity()) / format.channelCount;
    }

    Error Buffer::MergeInto(Buffer* other, unsigned frameCount)
    {
        if (!other)
            return NullDestination;

        if (format == other->format)
        {
            for (unsigned i = 0; i < frameCount * format.channelCount; i++)
                other->data[i] += data[i];
        }
        else
        {
            return ConvertAndMergeInto(other, frameCount);
        }

        return OK;
    }

    Error Buffer::ConvertAndMergeInto(Buffer* other, unsigned frameCount)
    {
        if (format.channelCount != 1 && format.channelCount != 2
            && other->format.channelCount != 1 && other->format.channelCount != 2)
            return UnsupportedFormat;

        Error error = OK;

        if (other->format.channelCount == 1)
            error = Converter::StereoToMono(this, other, frameCount);
        else
            error = Converter::MonoToStereo(this, other, frameCount);
        
        if (error)
            return error;

        if (other->format.frequency != format.frequency)
            error = Converter::Resample(this, other, frameCount);

        return error;
    }

    bool Buffer::TryAcquire()
    {
        return !assigned.test_and_set(std::memory_order_acquire);
    }

    void Buffer::Release()
    {
        assigned.clear(std::memory_order_release);
    }

    void Swap(Buffer*& a, Buffer*& b)
    {
        Buffer* tmp = a;
        a = b;
        b = tmp;
    }
}
