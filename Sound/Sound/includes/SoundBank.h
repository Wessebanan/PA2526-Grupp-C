#pragma once
#include "SoundFile.h"
namespace Sound
{
	class Bank
	{
	public:
		Bank();
		~Bank();
		File* GetFile(std::string Path);
	private:
		File* mpFiles[SOUND_MAX_BANK_FILES];
	};
}