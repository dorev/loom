#pragma once

#include "effectable.h"
#include "ibufferprovider.h"

namespace Loom
{
    enum class NodeType
    {
        Sound,
        Bus,
        Emitter
    };

    class Node : public Effectable
    {
    public:
        Node(IBufferProvider* bufferProvider)
            : bufferProvider(bufferProvider)
        {
        }

        virtual ~Node()
        {
        }

        virtual Error Read(Buffer*& destinationBuffer, unsigned frameCount) = 0;
        virtual Error VirtualRead(unsigned frameCount) = 0;
        virtual NodeType Type() const = 0;

        inline bool IsEmitter() const { return Type() == NodeType::Emitter; }
        inline bool IsSound() const { return Type() == NodeType::Sound; }
        inline bool IsBus() const { return Type() == NodeType::Bus; }

        IBufferProvider* bufferProvider;
    };
}
