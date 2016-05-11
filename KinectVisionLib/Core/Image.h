#pragma once

#include "Rect.h"
#include "ImageBase.h"

namespace KinectVisionLib{
    namespace Core{

        using namespace std;

        template<class T>
        class Image : public ImageBase
        {
        public:
            Image(int width, int height)
                : ImageBase(width, height)
            {
                this->stride = width;
                this->length = width * height * sizeof(T);
                this->buffer = new T[width * height];
                ZeroMemory(this->buffer, this->length);
            }

            Image(const Size& size)
                : Image(size.GetWidth(), size.GetHeight())
            {}

            Image(int width, int height, const T* buffer)
                : ImageBase(width, height)
            {
                this->stride = width;
                this->length = width * height * sizeof(T);
                this->buffer = new T[width * height];
                for (int i = 0; i < this->GetHeight(); i++)
                {
                    CopyMemory(this->buffer + i * this->stride, buffer + i * width, width * 2);
                }
            }

            Image(const Image& image)
                : Image(image.GetWidth(), image.GetHeight(), image.GetScan0())
            {
            }

            ~Image()
            {
                delete[] this->buffer;
                this->buffer = nullptr;
            }

            int GetLength() const { return length; }
            int GetStride() const { return stride; }
            T* GetScan0() { return buffer; }
            const T* GetScan0() const { return buffer; }

            T GetPixel(const Point& point) const {
                return *(GetScan0() + point.GetY() * GetStride() + point.GetX());
            }

            T GetPixelSafe(const Point& point) const {
                return IsValid(point) ? *(GetScan0() + point.GetY() * GetStride() + point.GetX()) : 0;
            }

            bool GetPixelInterpolation(float x, float y, T& result) const {
                int x0 = (int)x;
                int y0 = (int)y;

                if (x0 < 0 || x0 >= GetWidth() - 1 || y0 < 0 || y0 >= GetHeight() - 1)
                {
                    return false;
                }

                float p0 = GetPixel(Point(x0, y0));
                float p1 = GetPixel(Point(x0 + 1, y0));
                float p2 = GetPixel(Point(x0, y0 + 1));
                float p3 = GetPixel(Point(x0 + 1, y0 + 1));

                // Filter out invalid points. 
                // TODO: We may accept the 1 invalid point in 4 of them
                if (p0 == 0 || p1 == 0 || p2 == 0 || p3 == 0)
                {
                    return false;
                }

                float p = p0 * (x0 + 1 - x) * (y0 + 1 - y) +
                    p1 * (x - x0) * (y0 + 1 - y) +
                    p2 * (x0 + 1 - x) * (y - y0) +
                    p3 * (x - x0) * (y - y0);
                result = (T)p;
                return true;
            }

        public:
            void SetPixel(const Point& point, T pixel) {
                *(GetScan0() + point.GetY() * GetStride() + point.GetX()) = pixel;
            }

            void SetPixelSafe(const Point& point, T pixel) {
                if (IsValid(point))
                {
                    *(GetScan0() + point.GetY() * GetStride() + point.GetX()) = pixel;
                }
            }

        public:
            bool Test3x3Window(const Point& point, function<bool(T*, int)> test)
            {
                return TestNxNWindow(1, point, test, threshold);
            }

            bool Test5x5Window(const Point& point, function<int(T)> test, function<bool(int)> threshold)
            {
                // Extend 2 pixel on each direction
                return TestNxNWindow(2, point, test, threshold);
            }

            // Expend range amount on each direction
            bool TestNxNWindow(int range, const Point& point, function<int(T)> test, function<bool(int)> threshold)
            {
                int resultCount = 0;
                for (int i = point.GetY() - range; i <= point.GetY() + range; i++)
                {
                    const T* scan = GetScan0() + i * GetStride();
                    for (int j = point.GetX() - range; j <= point.GetX() + range; j++)
                    {
                        resultCount += test(*(scan + j));
                    }
                }
                return threshold(resultCount);
            }

            void ImageOperation(function<void(T*)> operation)
            {
                for (int i = 0; i < GetHeight(); i++)
                {
                    T* img = this->GetScan0() + i * this->GetStride();
                    for (int j = 0; j < GetWidth(); j++)
                    {
                        operation(img);
                        img++;
                    }
                }
            }

            void ImageOperation(function<void(int x, int y, T pixel)> operation) const
            {
                for (int i = 0; i < GetHeight(); i++)
                {
                    const T* img = this->GetScan0() + i * this->GetStride();
                    for (int j = 0; j < GetWidth(); j++)
                    {
                        operation(j, i, *img);
                        img++;
                    }
                }
            }

            template<typename T1, typename T2>
            void ImageOperation(const Image<T1>* op1, Image<T2>* op2, function<void(const T*, const T1*, T2*)> operation) const
            {
                assert(this->GetWidth() == op1->GetWidth() && this->GetWidth() == op2->GetWidth());
                assert(this->GetHeight() == op1->GetHeight() && this->GetHeight() == op2->GetHeight());
                for (int i = 0; i < GetHeight(); i++)
                {
                    const T* img = this->GetScan0() + i * this->GetStride();
                    const T1* op1Scan = op1->GetScan0() + i * op1->GetStride();
                    T2* op2Scan = op2->GetScan0() + i * op2->GetStride();
                    for (int j = 0; j < GetWidth(); j++)
                    {
                        operation(img, op1Scan, op2Scan);
                        img++;
                        op1Scan++;
                        op2Scan++;
                    }
                }
            }

            template<typename T1, typename T2, typename T3>
            void ImageOperation(const Image<T1>* op1, Image<T2>* op2, Image<T3>* op3, function<void(T*, const T1*, T2*, T3*)> operation)
            {
                assert(this->GetWidth() == op1->GetWidth() && this->GetWidth() == op2->GetWidth() && this->GetWidth() == op3->GetWidth());
                assert(this->GetHeight() == op1->GetHeight() && this->GetHeight() == op2->GetHeight() && this->GetHeight() == op3->GetHeight());
                for (int i = 0; i < GetHeight(); i++)
                {
                    T* img = this->GetScan0() + i * this->GetStride();
                    const T1* op1Scan = op1->GetScan0() + i * op1->GetStride();
                    T2* op2Scan = op2->GetScan0() + i * op2->GetStride();
                    T3* op3Scan = op3->GetScan0() + i * op3->GetStride();
                    for (int j = 0; j < GetWidth(); j++)
                    {
                        operation(img, op1Scan, op2Scan, op3Scan);
                        img++;
                        op1Scan++;
                        op2Scan++;
                        op3Scan++;
                    }
                }
            }

            template<typename T1>
            void ImageOperation(const Image<T1>* op1, function<void(T*, const T1*)> operation)
            {
                assert(GetWidth() == op1->GetWidth());
                assert(GetHeight() == op1->GetHeight());
                for (int i = 0; i < GetHeight(); i++)
                {
                    T* imgScan = this->GetScan0() + i * this->GetStride();
                    const T1* sourceScan = op1->GetScan0() + i * this->GetStride();
                    for (int j = 0; j < GetWidth(); j++)
                    {
                        operation(imgScan, sourceScan);
                        imgScan++;
                        sourceScan++;
                    }
                }
            }

        private:
            T* buffer;
            int length;
            int stride;
        };

    }
}
