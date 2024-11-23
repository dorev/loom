#pragma once

#include "node.h"

namespace Loom
{
    class Bus : public Node
    {
    public:
        Bus(IBufferProvider* bufferProvider)
            : Node(bufferProvider)
        {
        }

        NodeType Type() const override
        {
            return NodeType::Bus;
        }

        Error Read(Buffer*& destinationBuffer, unsigned frameCount) override
        {
            if (sources.empty())
                return OK;

            Buffer* buffer = nullptr;

            for (Node* source : sources)
            {
                Error error = source->Read(buffer, frameCount);
                if (error)
                    return error;
            }

            if (buffer)
            {
                Error error = ApplyEffects(buffer, frameCount, this);
                if (error)
                    return error;

                if (destinationBuffer)
                {
                    error = buffer->MergeInto(destinationBuffer, frameCount);
                    bufferProvider->ReturnBuffer(buffer);
                    return error;
                }
                else
                {
                    Swap(buffer, destinationBuffer);
                }
            }

            return OK;
        }

        Error VirtualRead(unsigned frameCount) override
        {
            for (Node* source : sources)
            {
                Error error = source->VirtualRead(frameCount);
                if (error)
                    return error;
            }

            return OK;
        }

        template <class T>
        void AddSource(T* source)
        {
            static_assert(std::is_base_of<Node, T>::value, "T must be derived from Source");
            sources.push_back(static_cast<Node*>(source));
        }

        std::vector<Node*> sources;
    };
}
