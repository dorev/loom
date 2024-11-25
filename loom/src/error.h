#pragma once

namespace Loom
{
    constexpr float Pi = 3.14159265358979323846f;

    enum Error : unsigned
    {
        OK = 0,
        Failed,
        NullBuffer,
        BufferTooSmall,
        NullDestination,
        NullSource,
        NullEffect,
        UnsupportedFormat,
        FormatMismatch,
        OutOfBound,
        InvalidParameter,
        InvalidSeek,
        EndOfFile,
        NotSupported,
        Invalid = unsigned(-1),
    };
}
