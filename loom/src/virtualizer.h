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
            buffer = bufferPool.GetBuffer();
            return OK;
        }

        void ReturnBuffer(Buffer*& buffer) override
        {
            bufferPool.ReturnBuffer(buffer);
            buffer = nullptr;
        }

        BufferPool bufferPool;
    };
}
