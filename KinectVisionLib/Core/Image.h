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
			//	//for (int i = 0; i < height; i++)
			//	//{
			//	//	uint16* scanLineCurrent = this->GetScan0() + i * this->GetStride();
			//	//	uint16* scanLineAbove = this->Get
			//	//	for (int j = 0; j < width; j++)
			//	//}
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
