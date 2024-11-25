#pragma once

#include "error.h"

namespace Loom
{
    class Buffer;

    class IDataNode
    {
    public:
        virtual ~IDataNode()
        {
        }

        virtual Error Read(Buffer* buffer, unsigned bufferFrameOffset, unsigned framePosition, unsigned frameCount, unsigned& framesRead) = 0;

        virtual Error GetFormat(Format& /*format*/)
        {
            return NotSupported;
        }

        virtual Error GetDuration(float& /*seconds*/)
        {
            return NotSupported;
        }

        virtual Error GetFrameCount(unsigned& /*frameCount*/)
        {
            return NotSupported;
        }
    };
}
