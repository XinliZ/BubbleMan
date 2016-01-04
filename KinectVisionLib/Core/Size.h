#pragma once

namespace KinectVisionLib
{
	namespace Core
	{
		class Size
		{
		public:
			Size(int width, int height)
			{
				this->width = width;
				this->height = height;
			}

			Size(const Size& size)
			{
				this->width = size.width;
				this->height = size.width;
			}

			int GetWidth() const { return width; }
			int GetHeight() const { return height; }

		private:
			int width;
			int height;
		};
	}
}