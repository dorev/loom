#pragma once

#include "bus.h"
#include "listener.h"

namespace Loom
{
    class Emitter : public Bus
    {
    public:
        Emitter(IBufferProvider* bufferProvider, Transform transform, Listener* listener)
            : Bus(bufferProvider)
            , transform(transform)
            , listener(listener)
        {
        }

        NodeType Type() const override
        {
            return NodeType::Emitter;
        }

        Transform transform;
        Listener* listener;
    };
}
