#pragma once

#include "Image.h"

namespace KinectVisionLib {
    namespace Core {
        using namespace std;

        class ProcessContext
        {
        public:
            ProcessContext() {}

            void AddDebugFrame(const wstring& name, shared_ptr<Image<uint16>> debugFrame)
            {
                debugFrames.insert(pair<wstring, shared_ptr<Image<uint16>>>(name, debugFrame));
            }

            void Clear()
            {
                debugFrames.clear();
            }

        public:
            map<wstring, shared_ptr<Image<uint16>>> debugFrames;
        };
    }
}