#include "RenderBuffer.h"

namespace graphics
{
	void RenderBuffer::Initialize(const unsigned int capacity, const unsigned int alignment)
	{	
		mAlignment = alignment;
		mCapacity = capacity;
		mUsage = 0;
		mpMemory = (char*)malloc(mCapacity);
	}	 
		 
	void RenderBuffer::Reset()
	{
		mUsage = 0;
	}

	char* RenderBuffer::GetBufferAddress(const unsigned int size)
	{
		char* pCurrent = mpMemory + mUsage;
		mUsage += PAD(size, mAlignment);
		return pCurrent;
	}

	unsigned int RenderBuffer::GetUsedMemory()
	{
		return mUsage;
	}

	void* RenderBuffer::GetStartAddress()
	{
		return (void*)mpMemory;
	}

	void RenderBuffer::Terminate()
	{
		free(mpMemory);
	}
}