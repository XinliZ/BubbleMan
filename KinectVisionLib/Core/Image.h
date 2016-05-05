#pragma once

#include "Rect.h"

namespace KinectVisionLib{
    namespace Core{

        using namespace std;

        template<class T>
        class Image
        {
        public:
            Image(int width, int height)
            {
                this->width = width;
                this->height = height;
                this->stride = width;
                this->length = width * height * 2;
                this->buffer = new T[width * height];
                ZeroMemory(this->buffer, this->length);
            }

            Image(const Size& size)
                : Image(size.GetWidth(), size.GetHeight())
            {}

            Image(int width, int height, const T* buffer)
            {
                this->width = width;
                this->height = height;
                this->stride = width;
                this->length = width * height * 2;
                this->buffer = new T[width * height];
                for (int i = 0; i < this->height; i++)
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
                delete this->buffer;
                this->buffer = nullptr;
            }

            int GetWidth() const { return width; }
            int GetHeight() const { return height; }
            int GetLength() const { return length; }
            int GetStride() const { return stride; }
            T* GetScan0() { return buffer; }
            const T* GetScan0() const { return buffer; }
            Rect GetRect() const { return Rect(0, 0, width, height); }
            Size GetSize() const { return Size(width, height); }

            T GetPixel(Point point) const {
                return *(GetScan0() + point.GetY() * GetStride() + point.GetX());
            }

            T GetPixelSafe(Point point) const {
                return IsValid(point) ? *(GetScan0() + point.GetY() * GetStride() + point.GetX()) : 0;
            }

        protected:
            void SetPixel(Point point, T pixel) {
                *(GetScan0() + point.GetY() * GetStride() + point.GetX()) = pixel;
            }

            void SetPixelSafe(Point point, T pixel) {
                if (IsValid(point))
                {
                    *(GetScan0() + point.GetY() * GetStride() + point.GetX()) = pixel;
                }
            }

        public:
            bool Test3x3Window(Point point, function<bool(T*, int)> test)
            {
                return TestNxNWindow(1, point, test, threshold);
            }

            bool Test5x5Window(Point point, function<int(T)> test, function<bool(int)> threshold)
            {
                // Extend 2 pixel on each direction
                return TestNxNWindow(2, point, test, threshold);
            }

            // Expend range amount on each direction
            bool TestNxNWindow(int range, Point point, function<int(T)> test, function<bool(int)> threshold)
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

            template<typename T1, typename T2>
            void ImageOperation(const Image<T1>* op1, Image<T2>* op2, function<void(const T*, const T1*, T2*)> operation) const
            {
                assert(this->width == op1->GetWidth() && this->width == op2->GetWidth());
                assert(this->height == op1->GetHeight() && this->height == op2->GetHeight());
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
                assert(this->width == op1->GetWidth() && this->width == op2->GetWidth() && this->width == op3->GetWidth());
                assert(this->height == op1->GetHeight() && this->height == op2->GetHeight() && this->height == op3->GetHeight());
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

            virtual void ToDisplay(uint8* buffer) const = 0;

        private:
            bool IsValid(Point point) const{
                return Rect(0, 0, width, height).Contains(point);
            }
        private:
            int width;
            int height;
            T* buffer;
            int length;
            int stride;
        };

    }
}
