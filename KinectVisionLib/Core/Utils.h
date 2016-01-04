#pragma once

namespace KinectVisionLib{
    namespace Core{
        class Utils{
        public:
            static uint8 ConvertToByte(uint32 value)
            {
                return value > 256 ? 256 : value;
            }
        };
    }
}