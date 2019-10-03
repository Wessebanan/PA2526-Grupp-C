#include "SoundBank.h"

Sound::Bank::Bank()
{
	for (int i = 0; i < SOUND_MAX_BANK_FILES; i++)
	{
		mpFiles[i] = nullptr;
	}
}

Sound::Bank::~Bank()
{
	for (int i = 0; i < SOUND_MAX_BANK_FILES; i++)
	{
		if (mpFiles[i] != nullptr)
		{
			delete mpFiles[i];
			mpFiles[i] = nullptr;
		}
	}
}

Sound::File* Sound::Bank::GetFile(std::string Path)
{
	for (int i = 0; i < SOUND_MAX_BANK_FILES; i++)
	{
		if (mpFiles[i] != nullptr)
		{
			if (mpFiles[i]->StringCompare(Path))
			{
				return mpFiles[i];
			}
		}
	}
	for (int i = 0; i < SOUND_MAX_BANK_FILES; i++)
	{
		if (mpFiles[i] == nullptr)
		{
			mpFiles[i] = new File();
			if (mpFiles[i]->Load(Path))
			{
				return mpFiles[i];
			}
			else
			{
				delete mpFiles[i];
				mpFiles[i] = nullptr;
				return nullptr;
			}
		}
	}
}
