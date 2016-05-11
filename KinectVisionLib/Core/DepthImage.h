#pragma once

#include "Image.h"
#include "Utils.h"

namespace KinectVisionLib{
    namespace Core{
        class DepthImage : public Image<uint16>
        {
        public:
            DepthImage(int width, int height)
                : Image<uint16>(width, height)
            {}

            DepthImage(const Size& size)
                : Image<uint16>(size)
            {}

            DepthImage(uint16* buffer, int width, int height)
                : Image<uint16>(width, height, buffer)
            {}

            DepthImage(const DepthImage& depthImage)
                : Image<uint16>(depthImage)
            {}

            virtual void RenderAsRGBA(uint8* buffer) const override
            {
                int minDepth = 500;
                int maxDepth = 4500;
                NormalizeDepthDataToBuffer(buffer, minDepth, maxDepth);
            }

            void NormalizeDepthDataToBuffer(uint8* buffer, uint16 minDepth, uint16 maxDepth) const
            {
                const uint32 MapDepthToByte = 4000 / 256;

                auto target = buffer;
                for (int i = 0; i < GetHeight(); i++)
                {
                    const uint16* scan0 = GetScan0() + i * GetStride();
                    for (int j = 0; j < GetWidth(); j++)
                    {
                        if (*scan0 == 0)
                        {
                            buffer[1] = 0xFF;        // Green
                            buffer[3] = 0xFF;
                        }
                        else if (*scan0 <= minDepth)
                        {
                            buffer[0] = (uint8)*scan0;        // Red
                            buffer[3] = 0xFF;
                        }
                        else if (*scan0 >= maxDepth)
                        {
                            buffer[2] = 0xFF;        // Blue
                            buffer[3] = 0xFF;
                        }
                        else
                        {
                            uint8 depth = Utils::ConvertToByte(*scan0 / MapDepthToByte);
                            buffer[0] = depth;
                            buffer[1] = depth;
                            buffer[2] = depth;
                            buffer[3] = 0xFF;
                        }

                        buffer += 4;
                        scan0++;
                    }
                }
            }


        };
    }
}