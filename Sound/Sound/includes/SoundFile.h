#pragma once
#include "SoundHelpers.h"
//#include "tinywav.h"
#include <string>

namespace Sound
{
	class FileData
	{
		// Following the canonical wave format header
		// http://soundfile.sapp.org/doc/WaveFormat/
		typedef struct WavHeader {
			uint32_t ChunkID;
			uint32_t ChunkSize;
			uint32_t Format;
			uint32_t Subchunk1ID;
			uint32_t Subchunk1Size;
			uint16_t AudioFormat;
			uint16_t NumChannels;
			uint32_t SampleRate;
			uint32_t ByteRate;
			uint16_t BlockAlign;
			uint16_t BitsPerSample;
			uint32_t Subchunk2ID;
			uint32_t Subchunk2Size;
		} WavHeader;
	public:
		FileData();
		~FileData();
		// This load ALL the sample data into memory
		// at once. In future implementations, this is 
		// NOT a good idea for large files, since we
		// have a limited memory budget
		bool LoadAll(std::string& Path);
		// Deletes all loaded sample data
		void Unload();
		// Get the amount of frames loaded
		// This takes into consideration amount of channels
		Samples GetFrameCount() const;
		// Get the amount of samples loaded
		Samples GetSampleCount() const;
		// Get the number of channels interleaved into
		// the sample data
		int GetNumChannels() const;
		// Get a pointer to the sample data
		float* GetDataPointer();
		// Compare two strings and return if equal
		bool StringIsEqual(std::string& Path);
	private:
		WavHeader mHeader;
		std::string mPath;
		float* mpData;
		Samples mSampleCount;
	};
}