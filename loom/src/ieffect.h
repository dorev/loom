#pragma once

#include "error.h"

namespace Loom
{
    class Buffer;
    class Node;

    class IEffect
    {
    public:
        virtual Error Process(Buffer* buffer, unsigned frameCount, Node* source) = 0;
    };
}
