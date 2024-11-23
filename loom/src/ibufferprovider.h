#pragma once

namespace Loom
{
    class Node;
    class Buffer;

    class IBufferProvider
    {
    public:
        virtual Error RequestBuffer(Buffer*& buffer, const Node* source) = 0;
        virtual void ReturnBuffer(Buffer*& buffer) = 0;
    };
}
