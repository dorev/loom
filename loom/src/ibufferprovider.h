#pragma once

namespace Loom
{
    class Node;
    class Buffer;

    class IBufferProvider
    {
    public:
        virtual ~IBufferProvider()
        {
        }

        virtual Error RequestBuffer(Buffer*& buffer, const Node* source) = 0;
        virtual Error ReturnBuffer(Buffer*& buffer) = 0;
        virtual Error GetFormat(Format&) const = 0;
    };
}
