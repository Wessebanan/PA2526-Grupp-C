#pragma once
#include <cmath>

inline unsigned int PAD(const unsigned int size, const unsigned int alignment)
{
	return (unsigned int)(ceil(size / (float)alignment) * alignment);
}

namespace graphics
{
	struct RenderBuffer
	{
		void Initialize(const unsigned int capacity, const unsigned int alignment);
		void Reset();

		char* GetBufferAddress(const unsigned int size);

		unsigned int GetUsedMemory();
		void* GetStartAddress();

		void Terminate();

	private:
		unsigned int mAlignment;
		unsigned int mCapacity;
		unsigned int mUsage;
		char* mpMemory;
	};
}