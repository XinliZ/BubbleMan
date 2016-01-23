#pragma once

namespace KinectVisionLib
{
    namespace Core
    {
        class Vector3
        {
        public:
            Vector3(float x, float y, float z)
            {
                this->x = x;
                this->y = y;
                this->z = z;
            }

            Vector3(const Vector3& v)
            {
                this->x = v.x;
                this->y = v.y;
                this->z = v.z;
            }

            Vector3 operator-(const Vector3& v) const
            {
                return Vector3(x - v.x, y - v.y, z - v.z);
            }

            Vector3 RotateTranslate(float rX, float rY, float rZ, float dX, float dY, float dZ)
            {
                float sX = sin(rX);
                float cX = cos(rX);
                float sY = sin(rY);
                float cY = cos(rY);
                float sZ = sin(rZ);
                float cZ = cos(rZ);

                auto newX = cY * cZ * this->x
                    + (cX * sZ + sX * sY * cZ) * this->y
                    + (sX * sZ - cX * sY * cZ) * this->z
                    + dX;

                auto newY = -cY * sZ * this->x
                    + (cX * cZ - sX * sY * sZ) * this->y
                    + (sX * cZ + cX * sY * sZ) * this->z
                    + dY;

                auto newZ = sY * this->x
                    - sX * cY * this->y
                    + cX * cY * this->z
                    + dZ;

                return Vector3(newX, newY, newZ);
            }

            float GetX() const { return x; }
            float GetY() const { return y; }
            float GetZ() const { return z; }

        private:
            float x;
            float y;
            float z;
        };
    }
}