#pragma once
#include "AudioFile.h"
namespace Audio
{
	class Bank
	{
	public:
		Bank();
		~Bank();
		File::SoundData* GetFile(std::string path);
		bool LoadMultipleFiles(const std::string* pPathArray, int count);
		File::SoundData* operator[](int index);
	private:
		File::SoundData* SetFileAtIndex(std::string path, int index);
		File::SoundData* mpFiles[SOUND_MAX_BANK_FILES];
	};
}