#pragma once
#include "AudioFile.h"
namespace Audio
{
	class Bank
	{
	public:
		Bank();
		~Bank();
		FileData* GetFile(std::string path);
		bool LoadMultipleFiles(const std::string* pPathArray, int count);
		FileData* operator[](int index);
	private:
		FileData* SetFileAtIndex(std::string path, int index);
		FileData* mpFiles[SOUND_MAX_BANK_FILES];
	};
}