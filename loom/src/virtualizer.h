#pragma once

#include "bufferpool.h"
#include "ibufferprovider.h"

namespace Loom
{
    class Virtualizer : public IBufferProvider
    {
    public:
        Error RequestBuffer(Buffer*& buffer, const Node* /*source*/) override
        {
            return bufferPool.GetBuffer(buffer);
        }

        Error ReturnBuffer(Buffer*& buffer) override
        {
            Error error = bufferPool.ReturnBuffer(buffer);
            buffer = nullptr;
            return error;
        }

        Error GetFormat(Format& format) const
        {
            return bufferPool.GetFormat(format);
        }

        BufferPool bufferPool;
    };
}
