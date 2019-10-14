#include "AudioFile.h"
Audio::File::SoundData::SoundData()
{
	mHeader = { 0 };
	mpData = nullptr;
	mSampleCount = 0;
}

Audio::File::SoundData::~SoundData()
{
	Unload();
}


bool Audio::File::SoundData::LoadAll(std::string& rPath)
{
	FILE* file;
	int subchunk_search_tries = 16;
	mPath = rPath;
	// Open the file and check if it exists/can be opened
	fopen_s(&file, rPath.c_str(), "rb");
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

void Audio::File::SoundData::Unload()
{
	if (mpData != nullptr)
	{
		delete[] mpData;
		mpData = nullptr;
	}
}

Audio::Samples Audio::File::SoundData::GetFrameCount() const
{
	return mSampleCount / mHeader.NumChannels;
}

Audio::Samples Audio::File::SoundData::GetSampleCount() const
{
	return mSampleCount;
}

int Audio::File::SoundData::GetNumChannels() const
{
	return mHeader.NumChannels;
}

float* Audio::File::SoundData::GetDataPointer()
{
	return mpData;
}

bool Audio::File::SoundData::StringIsEqual(std::string& rPath)
{
	return (rPath.compare(mPath) == 0);
}




bool Audio::File::MusicData::Init(std::string& rPath)
{
	int subchunk_search_tries = 16;
	// Open the file and check if it exists/can be opened
	fopen_s(&mpFileHandle, rPath.c_str(), "rb");
	if (mpFileHandle == nullptr)
		return false;
	// Read the Wave header
	fread(&mHeader, sizeof(WavHeader), 1, mpFileHandle);
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
		fseek(mpFileHandle, 4, SEEK_CUR);
		fread(&mHeader.Subchunk2ID, 4, 1, mpFileHandle);
	}
	// Get the size of the data
	fread(&mHeader.Subchunk2Size, 4, 1, mpFileHandle);
	// Get the size of the data
	fread(&mHeader.Subchunk2Size, 4, 1, mpFileHandle);

	// TODO Get the position of the actual data here!!!

	fgetpos(mpFileHandle, &mFileDataStart);

	mRefillBuffer = 0;
}
