#pragma once

#include "../DepthPixel.h"

namespace KinectVisionLib
{
	namespace Core
	{
		class MotionState
		{
			//friend DeltaMotionState;
		public:
			MotionState()
			{
				this->x = 0;
				this->y = 0;
				this->z = 0;
				this->roll = 0;
				this->yaw = 0;
				this->pitch = 0;
			}
			MotionState(float x, float y, float z, float roll, float yaw, float pitch)
			{
				this->x = x;
				this->y = y;
				this->z = z;
				this->roll = roll;
				this->yaw = yaw;
				this->pitch = pitch;
			}

			float GetX() const { return x; }
			float GetY() const { return y; }
			float GetZ() const { return z; }
			float GetRoll() const { return roll; }
			float GetYaw() const { return yaw; }
			float GetPitch() const { return pitch; }

			void TransformDepthPixel(int x, int y, int depth, int& x1, int& y1, int& depth1)
			{
				
			}

		private:
			float x;
			float y;
			float z;
			float roll;
			float yaw;
			float pitch;
		};

	}
}
