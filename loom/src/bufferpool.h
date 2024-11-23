#pragma once

#include "buffer.h"

#include <vector>

namespace Loom
{
    class BufferPool
    {
    public:
        BufferPool() = default;

        void Init(unsigned bufferCount, unsigned frameCount, Format format)
        {
            for (unsigned i = 0; i < bufferCount; ++i)
                buffers.push_back(new Buffer(frameCount, format));
        }

        Buffer* GetBuffer()
        {
            for (Buffer* buffer : buffers)
            {
                if (buffer->TryAcquire())
                    return buffer;
            }

            return nullptr;
        }

        void ReturnBuffer(Buffer*& buffer)
        {
            if (buffer)
            {
                buffer->Release();
                buffer = nullptr;
            }
        }

    private:
        std::vector<Buffer*> buffers;
    };
}


