#pragma once

#include "error.h"
#include <atomic>

namespace Loom
{
    class Buffer;
    class Node;

    enum class EffectType
    {
        Spatial,
        Filter,
    };

    class Effect
    {
    public:
        Effect(const char* name, bool bypass = false)
            : bypass(bypass)
            , name(name)
        {
        }

        virtual ~Effect()
        {
        }

        virtual Error Process(Buffer* buffer, unsigned frameCount, Node* source) = 0;
        virtual EffectType Type() const = 0;

        std::atomic<bool> bypass;
        const char* name;
    };
}
