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

            Image(Size size)
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

            T GetPixel(Point point) const {
                return *(GetScan0() + point.GetY() * GetStride() + point.GetX());
            }

            void SetPixel(Point point, uint16 pixel) {
                *(GetScan0() + point.GetY() * GetStride() + point.GetX()) = pixel;
            }

            T GetPixelSafe(Point point) const {
                return IsValid(point) ? *(GetScan0() + point.GetY() * GetStride() + point.GetX()) : 0;
            }

            void SetPixelSafe(Point point, uint16 pixel) {
                if (IsValid(point))
                {
                    *(GetScan0() + point.GetY() * GetStride() + point.GetX()) = pixel;
                }
            }

            bool Test3x3Window(Point point, function<bool(T*, int)> test)
            {
                T result[9];
                result[0] = GetPixel(point.Offset(-1, -1));
                result[1] = GetPixel(point.Offset(0, -1));
                result[2] = GetPixel(point.Offset(1, -1));
                result[3] = GetPixel(point.Offset(-1, 0));
                result[4] = GetPixel(point.Offset(0, 0));
                result[5] = GetPixel(point.Offset(1, 0));
                result[6] = GetPixel(point.Offset(-1, 1));
                result[7] = GetPixel(point.Offset(0, 1));
                result[8] = GetPixel(point.Offset(1, 1));
                return test(result, 9);
            }

            void ImageOperation(function<void(T*)> operation)
            {
                for (int i = 0; i < GetHeight(); i++)
                {
                    uint16* img = this->GetScan0() + i * this->GetStride();
                    for (int j = 0; j < GetWidth(); j++)
                    {
                        operation(img);
                        img++;
                    }
                }
            }

            template<typename T1, typename T2>
            void ImageOperation(shared_ptr<Image<T1>> op1, shared_ptr<Image<T2>> op2, function<void(T*, T1*, T2*)> operation)
            {
                assert(this->width == op1->GetWidth() && this->width == op2->GetWidth());
                assert(this->height == op1->GetHeight() && this->height == op2->GetHeight());
                for (int i = 0; i < GetHeight(); i++)
                {
                    uint16* img = this->GetScan0() + i * this->GetStride();
                    uint16* op1Scan = op1->GetScan0() + i * op1->GetStride();
                    uint16* op2Scan = op2->GetScan0() + i * op2->GetStride();
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
            void ImageOperation(shared_ptr<Image<T1>> op1, shared_ptr<Image<T2>> op2, shared_ptr<Image<T3>> op3, function<void(T*, T1*, T2*, T3*)> operation)
            {
                assert(this->width == op1->GetWidth() && this->width == op2->GetWidth() && this->width == op3->GetWidth());
                assert(this->height == op1->GetHeight() && this->height == op2->GetHeight() && this->height == op3->GetHeight());
                for (int i = 0; i < GetHeight(); i++)
                {
                    uint16* img = this->GetScan0() + i * this->GetStride();
                    uint16* op1Scan = op1->GetScan0() + i * op1->GetStride();
                    uint16* op2Scan = op2->GetScan0() + i * op2->GetStride();
                    uint16* op3Scan = op3->GetScan0() + i * op3->GetStride();
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
/*
            shared_ptr<Image> Apply(function<void(uint16*, uint16*, uint16*, uint16*)> transform, shared_ptr<Image> input, shared_ptr<Image> blackDotsMask)
            {
                shared_ptr<Image> result = std::make_shared<DepthImage>(this->width, this->height);
                uint16* scanLineResult = result->GetScan0();
                uint16* scanLineThis = this->GetScan0();
                uint16* scanLineInput = input->GetScan0();
                uint16* scanLineBlackDotsMask = blackDotsMask->GetScan0();

                for (int i = 0; i < height; i++)
                {
                    for (int j = 0; j < width; j++)
                    {
                        transform(scanLineResult + j, scanLineThis + j, scanLineInput + j, scanLineBlackDotsMask + j);
                    }

                    scanLineResult += result->GetStride();
                    scanLineThis += this->GetStride();
                    scanLineInput += input->GetStride();
                    scanLineBlackDotsMask += blackDotsMask->GetStride();
                }
                return result;
            }*/

            virtual void ToDisplay(uint8* buffer) const = 0;

            //void ApplyInPlace(function<void(uint16*, uint16*, uint16*, uint16*, uint16*, uint16*, uint16*, uint16*, uint16*, uint16*)> transform)
            //{
            //    //for (int i = 0; i < height; i++)
            //    //{
            //    //    uint16* scanLineCurrent = this->GetScan0() + i * this->GetStride();
            //    //    uint16* scanLineAbove = this->Get
            //    //    for (int j = 0; j < width; j++)
            //    //}
            //}

        private:
            bool IsValid(Point point) const{
                return Rect(0, 0, width, height).IsInside(point);
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
