#pragma once

#include <vector>

#include "effect.h"
#include "buffer.h"

namespace Loom
{
    class Effectable
    {
    public:
        Error AddEffect(Effect* effect)
        {
            if (!effect)
                return NullEffect;

            effects.push_back(effect);

            return OK;
        }

        Error ApplyEffects(Buffer* buffer, unsigned frameCount, Node* source)
        {
            for (Effect* effect : effects)
            {
                Error error = effect->Process(buffer, frameCount, source);
                if (error)
                    return error;
            }

            return OK;
        }

        std::vector<Effect*> effects;
    };
}
