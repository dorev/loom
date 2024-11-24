#pragma once

#include "emitter.h"
#include "idatanode.h"
#include "soundstate.h"

namespace Loom
{
    class Sound : public Node
    {
    public:
        Sound(IBufferProvider* bufferProvider, IDataNode* dataNode, Emitter* emitter, Listener* listener)
            : Node(bufferProvider)
            , state()
            , dataNode(dataNode)
            , listener(listener)
            , emitter(emitter)
            , loopStart(0)
            , loopEnd(Invalid)
        {
        }

        NodeType Type() const override
        {
            return NodeType::Sound;
        }

        void Play()
        {
            state.SetPlay(true);
        }

        bool IsPlaying() const
        {
            return state.IsPlaying();
        }

        bool IsLooping() const
        {
            return state.IsLooping();
        }

        void Pause()
        {
            state.SetPlay(false);
        }

        void Stop()
        {
            state.SetPlay(false);
            (void) state.SeekFrame(0);
        }

        Error SeekFrame(unsigned position)
        {
            unsigned frameCount = 0;

            Error error = dataNode->GetFrameCount(frameCount);
            if (error)
                return error;

            if (position > frameCount)
            {
                if (state.IsLooping())
                {
                    position %= frameCount;
                }
                else
                {
                    Stop();
                    return OK;
                }
            }

            state.SeekFrame(position);
            return OK;
        }

        Error SeekTime(float seconds)
        {
            Format format;
            Error error = bufferProvider->GetFormat(format);
            if (error)
                return error;

            unsigned framePosition = static_cast<unsigned>(seconds / static_cast<float>(format.frequency));

            return SeekFrame(framePosition);
        }

        Error Read(Buffer*& destinationBuffer, unsigned frameCount) override
        {
            Error error = OK;
            unsigned framesRead = 0;

            bool play = false;
            bool loop = false;
            unsigned framePosition = 0;
            state.Read(play, loop, framePosition);

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
                    error = LoopRead(buffer, framePosition, frameCount, framesRead);
                else
                    error = dataNode->Read(buffer, 0, framePosition, frameCount, framesRead);


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

            state.UpdateFramePosition(framePosition, framePosition + framesRead);

            return error;
        }

        Error VirtualRead(unsigned frameCount) override
        {
            bool play = false;
            bool loop = false;
            unsigned position = 0;
            state.Read(play, loop, position);

            if (play)
                state.UpdateFramePosition(position, position + frameCount);

            return OK;
        }

        Emitter* emitter;
        Listener* listener;
        IDataNode* dataNode;

private:
        Error LoopRead(Buffer* buffer, unsigned framePosition, unsigned frameCount, unsigned& framesRead)
        {
            Error error = dataNode->Read(buffer, 0, framePosition, frameCount, framesRead);

            while (error == EndOfFile && framesRead < frameCount)
                error = dataNode->Read(buffer, framesRead, 0, frameCount - framesRead, framesRead);

            return error;
        }

        SoundState state;
        unsigned loopStart;
        unsigned loopEnd;
    };
}
