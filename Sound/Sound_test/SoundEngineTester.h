#pragma once
#include "SoundEngine.h"

// This is my solution to manipulating member variables
// without exposing them as public member variables
namespace TestSpace
{
	namespace Sound 
	{
		// This test engine implementation sits ontop of
		// the sound engine and makes the GetChainBuffer
		// public. Feels like there may be a better solution.
		class Engine : public ::Sound::Engine
		{
		public:
			Engine() : ::Sound::Engine::Engine() {};
			::Sound::Buffer* GetChainBuffer(int Index)
			{
				return __GetChainBuffer(Index);
			}
		};
	}
}