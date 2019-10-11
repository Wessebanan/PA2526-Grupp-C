#include "AudioBank.h"
#include <iostream>
Audio::Bank::Bank()
{
	for (int i = 0; i < SOUND_MAX_BANK_FILES; i++)
	{
		mpFiles[i] = nullptr;
	}
}

Audio::Bank::~Bank()
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

Audio::File::SoundData* Audio::Bank::GetFile(std::string path)
{
	for (int i = 0; i < SOUND_MAX_BANK_FILES; i++)
	{
		if (mpFiles[i] != nullptr)
		{
			if (mpFiles[i]->StringIsEqual(path))
			{
				return mpFiles[i];
			}
		}
	}
	for (int i = 0; i < SOUND_MAX_BANK_FILES; i++)
	{
		if (mpFiles[i] == nullptr)
		{
			mpFiles[i] = new File::SoundData();
			if (mpFiles[i]->LoadAll(path))
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

bool Audio::Bank::LoadMultipleFiles(const std::string* pPathArray, int count)
{
	bool return_value = true;
	for (int i = 0; i < count; i++)
	{
		if (i >= SOUND_MAX_BANK_FILES)
		{
			std::cerr << "Audio bank reached maximum file capacity\n";
			return_value = false;
			break;
		}
#ifdef _DEBUG
		try
		{
#endif
			if (SetFileAtIndex(pPathArray[i], i) == nullptr)
			{
				std::cerr << pPathArray[i] << " could not be loaded\n";
				return_value = false;
			}
#ifdef _DEBUG
		}
		catch (std::bad_alloc e)
		{
			std::cerr << "Bad alloc! Probably reading outside Path Array (count too high)\n"
				<< e.what() << std::endl;
			return_value = false;
		}
#endif
	}


	return return_value;
}

Audio::File::SoundData* Audio::Bank::operator[](int index)
{
	if (index >= SOUND_MAX_BANK_FILES)
	{
		return nullptr;
	}
	return mpFiles[index];
}

Audio::File::SoundData* Audio::Bank::SetFileAtIndex(std::string path, int index)
{
	if (mpFiles[index] == nullptr)
	{
		mpFiles[index] = new File::SoundData();
		if (mpFiles[index]->LoadAll(path))
		{
			return mpFiles[index];
		}
		else
		{
			delete mpFiles[index];
			mpFiles[index] = nullptr;
			return nullptr;
		}
	}
	return nullptr;
}
