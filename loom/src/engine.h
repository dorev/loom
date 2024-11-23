#pragma once

#include "virtualizer.h"
#include "sound.h"

namespace Loom
{
    class Engine
    {
    public:
        Engine(unsigned bufferCount, unsigned bufferFrameCount, Format format)
            : defaultListener()
            , outputBus(&virtualizer, Transform(), & defaultListener)
        {
            virtualizer.bufferPool.Init(bufferCount, bufferFrameCount, format);
        }

        Error CreateSound(Sound*& sound, IDataNode* source, Bus* sink = nullptr)
        {
            if (!source)
                return NullSource;

            if (!sink)
                sink = &outputBus;
            
            if (sink->IsEmitter())
                sound = new Sound(&virtualizer, source, static_cast<Emitter*>(sink), &defaultListener);
            else
                sound = new Sound(&virtualizer, source, nullptr, &defaultListener);

            sink->AddSource(sound);

            return OK;
        }

        Error CreateEmitter(Emitter*& emitter, Transform transform = Transform(), Bus* sink = nullptr, Listener* listener = nullptr)
        {
            if (!sink)
                sink = &outputBus;

            if (!listener)
                listener = &defaultListener;

            emitter = new Emitter(&virtualizer, transform, listener);

            sink->AddSource(emitter);

            return OK;
        }

        Listener defaultListener;
        Virtualizer virtualizer;
        Emitter outputBus;
    };
}
