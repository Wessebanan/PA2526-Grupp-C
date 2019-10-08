#include "SoundFile.h"
Sound::FileData::FileData()
{
	mHeader = { 0 };
	mpData = nullptr;
	mSampleCount = 0;
}

Sound::FileData::~FileData()
{
	Unload();
}


bool Sound::FileData::LoadAll(std::string& Path)
{
	FILE* file;
	int subchunk_search_tries = 16;
	mPath = Path;
	// Open the file and check if it exists/can be opened
	fopen_s(&file, Path.c_str(), "rb");
	if (file == nullptr)
		return false;
	// Read the Wave header
	fread(&mHeader, sizeof(WavHeader), 1, file);
	// Sometimes the fmt chunk can have extra parameters
	// thus displacing the data chunk.
	// Search for the subchunk id "data" to correctly
	// read the actual data.
	while (mHeader.Subchunk2ID != 1635017060) {   // "data"
		// In case the wav is incorrectly formated or
		// corrupted, this is a fail safe to not get stuck
		// in an infinite loop
		subchunk_search_tries--;
		if (subchunk_search_tries <= 0)
		{
			return false;
		}
		// Progress 4 bytes and read again
		fseek(file, 4, SEEK_CUR);
		fread(&mHeader.Subchunk2ID, 4, 1, file);
	}
	// Get the size of the data
	fread(&mHeader.Subchunk2Size, 4, 1, file);

	// The size is in bits, convert to amount of samples
	mSampleCount = mHeader.Subchunk2Size / sizeof(float);
	// Allocate space for the all samples and read
	mpData = new float[mSampleCount];
	fread(mpData, sizeof(float), mSampleCount, file);
	// Close file
	fclose(file);

	return true;
}

void Sound::FileData::Unload()
{
	if (mpData != nullptr)
	{
		delete[] mpData;
		mpData = nullptr;
	}
}

Sound::Samples Sound::FileData::GetFrameCount() const
{
	return mSampleCount / mHeader.NumChannels;
}

Sound::Samples Sound::FileData::GetSampleCount() const
{
	return mSampleCount;
}

int Sound::FileData::GetNumChannels() const
{
	return mHeader.NumChannels;
}

float* Sound::FileData::GetDataPointer()
{
	return mpData;
}

bool Sound::FileData::StringIsEqual(std::string& Path)
{
	return (Path.compare(mPath) == 0);
}
