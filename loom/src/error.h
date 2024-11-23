#pragma once

namespace Loom
{
    constexpr float Pi = 3.14159265358979323846f;

    enum Error : unsigned
    {
        OK = 0,
        Failed,
        NullBuffer,
        NullDestination,
        NullSource,
        FormatMismatch,
        NullEffect,
        InvalidParameter,
        EndOfFile,
        BufferTooSmall
    };
}
