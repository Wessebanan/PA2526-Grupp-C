#pragma onc
#include "SoundHelpers.h"
#include "tinywav.h"
#include <string>

namespace Sound
{
	class File
	{
	public:
		File();
		~File();
		bool Load(std::string& Path);
		void Unload();
		Samples GetFrameCount() const;
		float* GetDataPointer();
		bool StringCompare(std::string& Path);
	private:
		TinyWav mWav;
		std::string mPath;
		float* mpData;
		Samples mFrameCount;
	};
}