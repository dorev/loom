#pragma once

#include "emitter.h"
#include "idatanode.h"

namespace Loom
{
    class Sound : public Node
    {
    public:
        Sound(IBufferProvider* bufferProvider, IDataNode* dataNode, Emitter* emitter, Listener* listener)
            : Node(bufferProvider)
            , dataNode(dataNode)
            , listener(listener)
            , emitter(emitter)
            , play(false)
            , position(0)
            , loop(false)
            , loopStart(0)
            , loopEnd(unsigned(-1))
        {
        }

        NodeType Type() const override
        {
            return NodeType::Sound;
        }

        Error Read(Buffer*& destinationBuffer, unsigned frameCount) override
        {
            Error error = OK;
            unsigned framesRead = 0;

            if (!play)
                return error;

            Buffer* buffer = nullptr;
            error = bufferProvider->RequestBuffer(buffer, this);

            if (buffer)
            {
                if (!buffer->CanContainXFrames(frameCount))
                {
                    buffer->Release();
                    return BufferTooSmall;
                }

                if (loop)
                    error = LoopRead(buffer, position, frameCount, framesRead);
                else
                    error = dataNode->Read(buffer, 0, position, frameCount, framesRead);


                if (error != OK && error != EndOfFile)
                    return error;

                error = ApplyEffects(buffer, frameCount, this);
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
            else
            {
                // No buffer provided by engine, swallow error for now
                error = OK;
            }

            position += framesRead;
            return error;
        }

        Error LoopRead(Buffer* buffer, unsigned seekPosition, unsigned frameCount, unsigned& framesRead)
        {
            Error error = dataNode->Read(buffer, 0, seekPosition, frameCount, framesRead);

            while (error == EndOfFile && framesRead < frameCount)
                error = dataNode->Read(buffer, framesRead, 0, frameCount - framesRead, framesRead);

            return error;
        }

        Error VirtualRead(unsigned frameCount) override
        {
            if (play)
                position += frameCount;

            return OK;
        }

        bool play;
        unsigned position;
        bool loop;
        unsigned loopStart;
        unsigned loopEnd;

        Emitter* emitter;
        Listener* listener;
        IDataNode* dataNode;
    };
}
