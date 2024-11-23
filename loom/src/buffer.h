#pragma once

#include <algorithm>
#include <vector>
#include <atomic>

#include "error.h"
#include "format.h"

namespace Loom
{
    class Buffer
    {
    public:
        Buffer(unsigned frameCount, Format format)
            : format(format)
            , assigned()
        {
            Reset(frameCount);
        }

        void Reset(unsigned frameCount)
        {
            if (!CanContainXFrames(frameCount))
                data.resize(frameCount * format.channelCount);

            std::fill(data.begin(), data.end(), 0.0f);
        }

        bool CanContainXFrames(unsigned frameCount) const
        {
            return data.capacity() >= (frameCount * format.channelCount);
        }

        unsigned FrameCapacity() const
        {
            return static_cast<unsigned>(data.capacity()) / format.channelCount;
        }

        Error MergeInto(Buffer* other, unsigned frameCount)
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

        Error ConvertAndMergeInto(Buffer* /*other*/, unsigned /*frameCount*/)
        {
            //
            // Insert format conversion library here!
            //

            return FormatMismatch;
        }

        bool TryAcquire()
        {
            return !assigned.test_and_set(std::memory_order_acquire);
        }

        void Release()
        {
            assigned.clear(std::memory_order_release);
        }

        std::vector<float> data;
        Format format;
        std::atomic_flag assigned;
    };

    void Swap(Buffer*& a, Buffer*& b)
    {
        Buffer* tmp = a;
        a = b;
        b = tmp;
    }
}
