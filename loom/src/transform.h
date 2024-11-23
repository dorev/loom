#pragma once

namespace Loom
{
    class Transform
    {
    public:
        Transform(float x = 0, float y = 0, float z = 0, float qw = 1, float qx = 0, float qy = 0, float qz = 0)
            : x(x)
            , y(y)
            , z(z)
            , qw(qw)
            , qx(qx)
            , qy(qy)
            , qz(qz)
        {
        }

        inline float ForwardX() const { return 2.0f * (qw * qx + qy * qz); }
        inline float ForwardY() const { return 2.0f * (qw * qy - qz * qx); }
        inline float ForwardZ() const { return 1.0f - 2.0f * (qx * qx + qy * qy); }

        float x, y, z;
        float qw, qx, qy, qz;
    };
}
