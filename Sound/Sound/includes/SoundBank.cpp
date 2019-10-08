#include "SoundBank.h"
#include <iostream>
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

Sound::FileData* Sound::Bank::GetFile(std::string Path)
{
	for (int i = 0; i < SOUND_MAX_BANK_FILES; i++)
	{
		if (mpFiles[i] != nullptr)
		{
			if (mpFiles[i]->StringIsEqual(Path))
			{
				return mpFiles[i];
			}
		}
	}
	for (int i = 0; i < SOUND_MAX_BANK_FILES; i++)
	{
		if (mpFiles[i] == nullptr)
		{
			mpFiles[i] = new FileData();
			if (mpFiles[i]->LoadAll(Path))
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

bool Sound::Bank::LoadMultipleFiles(const std::string* pPathArray, int Count)
{
	bool return_value = true;
	for (int i = 0; i < Count; i++)
	{
		if (i >= SOUND_MAX_BANK_FILES)
		{
			std::cerr << "Sound bank reached maximum file capacity\n";
			return_value = false;
			break;
		}
		if (SetFileAtIndex(pPathArray[i], i) == nullptr)
		{
			std::cerr << pPathArray[i] << " could not be loaded\n";
			return_value = false;
		}
	}


	return return_value;
}

Sound::FileData* Sound::Bank::operator[](int Index)
{
	if (Index >= SOUND_MAX_BANK_FILES)
	{
		return nullptr;
	}
	return mpFiles[Index];
}

Sound::FileData* Sound::Bank::SetFileAtIndex(std::string Path, int Index)
{
	if (mpFiles[Index] == nullptr)
	{
		mpFiles[Index] = new FileData();
		if (mpFiles[Index]->LoadAll(Path))
		{
			return mpFiles[Index];
		}
		else
		{
			delete mpFiles[Index];
			mpFiles[Index] = nullptr;
			return nullptr;
		}
	}
	return nullptr;
}
