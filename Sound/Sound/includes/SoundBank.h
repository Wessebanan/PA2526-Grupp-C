#pragma once
#include "SoundFile.h"
namespace Sound
{
	class Bank
	{
	public:
		Bank();
		~Bank();
		FileData* GetFile(std::string Path);
		bool LoadMultipleFiles(const std::string* pPathArray, int Count);
		FileData* operator[](int Index);
	private:
		FileData* SetFileAtIndex(std::string Path, int Index);
		FileData* mpFiles[SOUND_MAX_BANK_FILES];
	};
}