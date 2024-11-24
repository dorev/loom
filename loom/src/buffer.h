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
        Buffer(unsigned frameCount, Format format);
        void Reset(unsigned frameCount);
        bool CanContainXFrames(unsigned frameCount) const;
        unsigned FrameCapacity() const;
        Error MergeInto(Buffer* other, unsigned frameCount);
        Error ConvertAndMergeInto(Buffer* other, unsigned frameCount);
        bool TryAcquire();
        void Release();

        std::vector<float> data;
        Format format;
        std::atomic_flag assigned;
    };

    void Swap(Buffer*& a, Buffer*& b);
}
