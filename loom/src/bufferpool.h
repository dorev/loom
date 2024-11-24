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

        Error GetBuffer(Buffer*& bufferRequest)
        {
            for (Buffer* buffer : buffers)
            {
                if (buffer->TryAcquire())
                {
                    bufferRequest = buffer;
                    return OK;
                }
            }

            bufferRequest = nullptr;
            return OK;
        }

        Error ReturnBuffer(Buffer*& returnedBuffer)
        {
            if (!returnedBuffer)
                return NullBuffer;

            bool bufferFound = false;
            for (const Buffer* buffer : buffers)
            {
                if (buffer == returnedBuffer)
                {
                    bufferFound = true;
                    break;
                }
            }

            if (!bufferFound)
                return InvalidParameter;

            returnedBuffer->Release();
            returnedBuffer = nullptr;

            return OK;
        }

        Error GetFormat(Format& format) const
        {
            if (buffers.empty())
                return NullBuffer;

            format = buffers[0]->format;
            return OK;
        }

    private:
        std::vector<Buffer*> buffers;
    };
}


