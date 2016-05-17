#pragma once

#include "Image.h"
#include "Utils.h"

namespace KinectVisionLib
{
    namespace Core{
        class DifferencialImage : public Image<uint16>
        {
        public:
            DifferencialImage(int width, int height)
                : Image(width, height)
            {}

            virtual void RenderAsRGBA(uint8* buffer) const override
            {
                NormalizeMidDataToBuffer(buffer);
            }

            void NormalizeMidDataToBuffer(uint8* bitmapBuffer) const
            {
                int index = 0;
                const uint32 MapDepthToByte = 4;
                for (int i = 0; i < GetHeight(); i++)
                {
                    const uint16* scan0 = GetScan0() + i * GetStride();
                    for (int j = 0; j < GetWidth(); j++)
                    {
                        if (*scan0 > 0x8000)
                        {
                            uint8 depth = Utils::ConvertToByte((*scan0 - 0x8000) / MapDepthToByte);
                            bitmapBuffer[index * 4] = depth;
                            bitmapBuffer[index * 4 + 1] = 0;
                            bitmapBuffer[index * 4 + 2] = 0;
                            bitmapBuffer[index * 4 + 3] = 0xFF;
                        }
                        else
                        {
                            auto depth = Utils::ConvertToByte((0x8000 - *scan0) / MapDepthToByte);
                            bitmapBuffer[i * 4] = 0;
                            bitmapBuffer[i * 4 + 1] = depth;
                            bitmapBuffer[i * 4 + 2] = 0;
                            bitmapBuffer[i * 4 + 3] = 0xFF;
                        }

                        index += 4;
                        scan0++;
                    }
                }

            }
        };
    }
}
