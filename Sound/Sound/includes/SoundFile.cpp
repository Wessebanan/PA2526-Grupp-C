#include "SoundFile.h"
Sound::File::File()
{
	mpData = nullptr;
}

Sound::File::~File()
{
	Unload();
}

bool Sound::File::Load(std::string& Path)
{
	mPath = Path;
	if (!tinywav_open_read(&mWav, Path.c_str(), TW_INTERLEAVED, TW_FLOAT32))
	{
		return false;
	}
}

void Sound::File::Unload()
{
	//drwav_uninit(mWav);
	if (mpData != nullptr)
	{
		free(mpData);
		mpData = nullptr;
	}
}

Sound::Samples Sound::File::GetFrameCount() const
{
	return mFrameCount;
}

float* Sound::File::GetDataPointer()
{
	return mpData;
}

bool Sound::File::StringCompare(std::string& Path)
{
	return Path.compare(mPath);
}
