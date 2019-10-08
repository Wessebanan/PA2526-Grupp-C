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
	private:
		FileData* mpFiles[SOUND_MAX_BANK_FILES];
	};
}