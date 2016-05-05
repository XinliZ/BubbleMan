#pragma once

#include "Image.h"
#include "ImageBuffer.h"
#include "Rect.h"

namespace KinectVisionLib
{
    namespace Core
    {
        template<class T>
        class LocalImage : public Image<T>
        {
        public:
            LocalImage(Rect boundingBox)
                : Image<T>(boundingBox.GetSize())
                , boundingBox(boundingBox)
            {}

            const Rect& GetBoundingBox() const
            {
                return boundingBox;
            }

        public:

            template<typename T1>
            void ImageOperation(const Image<T1>* op1, const Rect& rect, function<void(T*, const T1*)> operation)
            {
                assert(rect <= op1->GetSize());
                
                for (int i = 0; i < GetHeight(); i++)
                {
                    T* imgScan = this->GetScan0() + i * this->GetStride();
                    const T1* sourceScan = op1->GetScan0() + (i + rect.GetTop()) * this->GetStride() + rect.GetLeft();
                    for (int j = 0; j < GetWidth(); j++)
                    {
                        operation(imgScan, sourceScan);
                        imgScan++;
                        sourceScan++;
                    }
                }
            }

            template<typename T1, typename T2>
            void ImageOperation(const Image<T1>* origin, ImageBuffer<T2>* target, function<void(const T*, const T1*, int x, int y, ImageBuffer<T2>* target)> operation) const
            {
                assert(boundingBox <= origin->GetSize());

                for (int i = 0; i < GetHeight(); i++)
                {
                    const T* maskScan = this->GetScan0() + i * this->GetStride();
                    const T1* originScan = origin->GetScan0() + (i + boundingBox.GetTop()) * origin->GetStride() + boundingBox.GetLeft();
                    for (int j = 0; j < GetWidth(); j++)
                    {
                        operation(maskScan, originScan, j, i, target);

                        maskScan++;
                        originScan++;
                    }
                }
            }

        private:
            Rect boundingBox;
        };
    }
}