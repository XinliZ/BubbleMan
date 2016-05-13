#pragma once

namespace KinectVisionLib
{
    namespace Core
    {
        class GlobalConsts
        {
        public:
            // The distance between kinect image plane to the origin. 
            //    depth / D0 == dx / 1 (1 interpixel distance)
            static const float KinectD0;
        };
    }
}