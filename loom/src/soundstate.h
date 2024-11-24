#pragma once

#include <atomic>

namespace Loom
{
    class SoundState
    {
    public:

        SoundState(bool playing = false, bool looping = false, unsigned framePosition = 0)
            : state(
                (playing ? PlayMask : 0) |
                (looping ? LoopMask : 0) |
                (static_cast<uint64_t>(framePosition) & FramePositionMask)
            )
        {
        }

        SoundState(const SoundState& other)
            :state(other.state.load())
        {
        }

        SoundState& operator=(const SoundState& other)
        {
            state.store(other.state.load());
        }

        void Read(bool& play, bool& loop, unsigned& position)
        {
            SoundState currentState(*this);
            play = IsPlaying();
            loop = IsLooping();
            position = GetFramePosition();
        }

        inline void SetPlay(bool play)
        {
            if (play)
                state.fetch_or(PlayMask);
            else
                state.fetch_and(~PlayMask);
        }

        inline void SetLoop(bool loop)
        {
            if (loop)
                state.fetch_or(LoopMask);
            else
                state.fetch_and(~LoopMask);
        }

        inline void SeekFrame(unsigned framePosition)
        {
            uint64_t newPositionMasked = static_cast<uint64_t>(framePosition) & FramePositionMask;
            uint64_t currentState;

            do
            {
                currentState = state.load();
            }
            while (!state.compare_exchange_weak(currentState, (currentState & ~FramePositionMask) | newPositionMasked));
        }

        inline bool UpdateFramePosition(unsigned oldPosition, unsigned newPosition)
        {
            uint64_t newPositionMasked = static_cast<uint64_t>(newPosition) & FramePositionMask;
            uint64_t currentState;
            uint64_t newState;

            do
            {
                currentState = state.load();

                uint64_t currentPosition = currentState & FramePositionMask;

                if (currentPosition != static_cast<uint64_t>(oldPosition))
                    return false; // Position has been modified by another thread

                newState = (currentState & ~FramePositionMask) | newPositionMasked;

            }
            while (!state.compare_exchange_weak(currentState, newState));

            return true;
        }

        inline bool IsPlaying() const
        {
            return (state.load() & PlayMask) != 0;
        }

        inline bool IsLooping() const
        {
            return (state.load() & LoopMask) != 0;
        }

        inline uint32_t GetFramePosition() const
        {
            return static_cast<uint32_t>(state.load() & FramePositionMask);
        }

    private:
        static constexpr uint64_t PlayMask = 0x8000000000000000;
        static constexpr uint64_t LoopMask = 0x4000000000000000;
        static constexpr uint64_t FramePositionMask = 0x00000000FFFFFFFF;

        std::atomic<uint64_t> state;
    };
}
