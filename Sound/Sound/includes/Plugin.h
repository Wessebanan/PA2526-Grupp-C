#pragma once
#include "SoundHelpers.h"

namespace Sound
{
	namespace Plugin
	{
		// Base plugin structure
		class Plugin
		{
		public:
			Plugin()
			{
				mpNext = nullptr;
			}
			// Once one plugin gets deleted, the chain of
			// plugins gets deleted
			~Plugin() {
				if (mpNext != nullptr)
				{
					delete mpNext;
					mpNext = nullptr;
				}
			}
			virtual void Process(Samples Start, Samples Count, float* Data, int Channels) = 0;
		protected:
			Plugin* mpNext;
		};
	}
}