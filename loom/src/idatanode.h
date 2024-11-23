#pragma once

#include "error.h"

namespace Loom
{
    class Buffer;

    class IDataNode
    {
    public:
        virtual Error Read(Buffer* buffer, unsigned bufferFrameOffset, unsigned seekFramePosition, unsigned frameCount, unsigned& framesRead) = 0;
    };
}
