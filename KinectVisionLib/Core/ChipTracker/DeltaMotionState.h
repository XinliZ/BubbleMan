#pragma once

#include "MotionState.h"

namespace KinectVisionLib
{
	namespace Core
	{
		class DeltaMotionState
		{
		public:
			DeltaMotionState()
			{}

			MotionState GenerateNew(MotionState& motionState)
			{
				return MotionState(
					motionState.GetX() + dX,
					motionState.GetY() + dY,
					motionState.GetZ() +dZ,
					motionState.GetRoll() + dRoll,
					motionState.GetYaw() + dYaw,
					motionState.GetPitch() + dPitch);
			}

			DepthPixel TransformDepthPixel(const DepthPixel& center, const DepthPixel& pixel)
			{
				return DepthPixel(0, 0, 0);
			}

			//MotionState GenerateNewWithVariation(MotionState motionState, )
			//{
			//	//return MotionState(motionState.X + dx, motionState.Y + dY)
			//}

		private:
			float dX;
			float dY;
			float dZ;
			float dRoll;
			float dYaw;
			float dPitch;
		};
	}
}