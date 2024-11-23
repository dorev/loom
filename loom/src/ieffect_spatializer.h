#pragma once

#include <cmath>

#include "ieffect.h"
#include "node.h"
#include "emitter.h"
#include "sound.h"

namespace Loom
{
    // Thank you ChatGPT!
    class Spatializer : public IEffect
    {
    public:
        Spatializer(float maxDistance = 100)
            : maxDistance(maxDistance)
        {
        }

        Error Process(Buffer* buffer, unsigned frameCount, Node* source) override
        {
            if (!buffer)
                return NullBuffer;

            // Assume stereo output (2 channels)
            if (buffer->format.channelCount != 2)
                return FormatMismatch;

            Emitter* emitter = nullptr;
            Listener* listener = nullptr;

            switch (source->Type())
            {
            case NodeType::Sound:
                emitter = static_cast<Sound*>(source)->emitter;
                listener = static_cast<Sound*>(source)->listener;
                break;

            case NodeType::Emitter:
                emitter = static_cast<Emitter*>(source);
                listener = static_cast<Emitter*>(source)->listener;
                break;

            case NodeType::Bus:
            default:
                return InvalidParameter;
            }

            const Transform& emitterTransform = emitter->transform;
            const Transform& listenerTransform = listener->transform;

            // Calculate listener's forward direction from quaternion
            float listenerForwardX = listenerTransform.ForwardX();
            float listenerForwardZ = listenerTransform.ForwardZ();

            // Normalize listener forward std::vector
            float forwardMagnitude = std::sqrt(listenerForwardX * listenerForwardX + listenerForwardZ * listenerForwardZ);
            if (forwardMagnitude > 0.0f)
            {
                listenerForwardX /= forwardMagnitude;
                listenerForwardZ /= forwardMagnitude;
            }

            // Calculate relative position
            float relativeX = emitterTransform.x - listenerTransform.x;
            float relativeZ = emitterTransform.z - listenerTransform.z;

            // Distance-based attenuation
            float distance = std::sqrt(relativeX * relativeX + relativeZ * relativeZ);
            float attenuation = 1.0f - std::clamp(distance / maxDistance, 0.0f, 1.0f);

            // Normalize relative position std::vector
            float relativeMagnitude = std::sqrt(relativeX * relativeX + relativeZ * relativeZ);
            if (relativeMagnitude > 0.0f)
            {
                relativeX /= relativeMagnitude;
                relativeZ /= relativeMagnitude;
            }

            // Calculate the angle between listener's forward std::vector and emitter's relative position
            float dotProduct = listenerForwardX * relativeX + listenerForwardZ * relativeZ;
            float angle = std::acos(std::clamp(dotProduct, -1.0f, 1.0f));
            float crossProduct = listenerForwardX * relativeZ - listenerForwardZ * relativeX;

            // Determine side of panning (-1.0 for left, 1.0 for right)
            float pan = 0.0f;
            if (std::abs(crossProduct) < 1e-6) // Handle emitters directly in front/behind
                pan = (relativeZ > 0) ? 0.0f : 1.0f; // Center for front, right for behind
            else
                pan = (crossProduct < 0.0f) ? -std::sin(angle) : std::sin(angle);

            // Calculate stereo gains
            float leftGain = attenuation * std::cos((Pi / 4.0f) * (1.0f + pan));
            float rightGain = attenuation * std::sin((Pi / 4.0f) * (1.0f + pan));

            // Apply the gains to each frame
            for (unsigned i = 0; i < frameCount; ++i)
            {
                buffer->data[i * 2] *= leftGain;        // Left channel
                buffer->data[i * 2 + 1] *= rightGain;  // Right channel
            }

            return OK;
        }

    private:
        float maxDistance;
    };
}