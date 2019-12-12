#include "AudioMixer.h"
#include <cmath>
#include <complex>
#include <valarray>

void Audio::Music::Manager::ReplaceMusic(Message& rMessage, MusicVoiceData* pTarget)
{
	if ((rMessage.flags & M_DATA_MASK) == M_DATA_AS_PARAMETER)
	{
		pTarget->Sampler.SetFileAndReset(rMessage.data._FileData);
	}
}

void Audio::Music::Manager::SetGain(Message& rMessage, MusicVoiceData* pTarget)
{
	if ((rMessage.flags & M_DATA_MASK) == M_DATA_AS_PARAMETER)
	{
		pTarget->Gain.SetGain(rMessage.data._float);
	}
}

void Audio::Music::Manager::FadeIn(Message& rMessage, MusicVoiceData* pTarget)
{
	if ((rMessage.flags & M_DATA_MASK) == M_DATA_AS_PARAMETER)
	{
		pTarget->Gain.FadeToFull(rMessage.data._ulong);
	}
}

void Audio::Music::Manager::FadeOut(Message& rMessage, MusicVoiceData* pTarget)
{
	if ((rMessage.flags & M_DATA_MASK) == M_DATA_AS_PARAMETER)
	{
		pTarget->Gain.FadeToEmpty(rMessage.data._ulong);
	}
}

void Audio::Music::Manager::SetSpeed(Message& rMessage, MusicVoiceData* pTarget)
{
	if ((rMessage.flags & M_DATA_MASK) == M_DATA_AS_PARAMETER)
	{
		pTarget->Sampler.SetPlayRate(rMessage.data._float);
	}
}

void Audio::Music::Manager::ProcessMusicMessages()
{
	Music::Message temp_message;
	while (mMusicMessageBuffer.remove(&temp_message))
	{
		// If the message is to change the tempo,
		// we'd like to set it to all music voices at
		// the same time
		if ((temp_message.flags & M_FUNC_MASK) == M_FUNC_SET_SPEED)
		{
			SetSpeed(temp_message, &mMainData);
			SetSpeed(temp_message, &mSecondaryData);
			SetSpeed(temp_message, &mSubData);
			continue;
		}

		// Select target
		Manager::MusicVoiceData* target_data = nullptr;
		switch (temp_message.flags & M_TARGET_MASK)
		{
		case M_TARGET_MAIN:
			target_data = &mMainData;
			break;
		case M_TARGET_SECONDARY:
			target_data = &mSecondaryData;
			break;
		case M_TARGET_SUB:
			target_data = &mSubData;
			break;
		}

		// Call function
		switch (temp_message.flags & M_FUNC_MASK)
		{
		case M_FUNC_REPLACE_MUSIC:
			ReplaceMusic(temp_message, target_data);
			break;
		case M_FUNC_SET_GAIN:
			SetGain(temp_message, target_data);
			break;
		case M_FUNC_FADE_IN:
			FadeIn(temp_message, target_data);
			break;
		case M_FUNC_FADE_OUT:
			FadeOut(temp_message, target_data);
			break;
		}

		// Syncing (Could be done more elegantly)
		if ((temp_message.flags & M_SYNC_MASK) == M_SYNC_THIS_WITH_OTHER)
		{
			switch (temp_message.flags & M_TARGET_MASK)
			{
			case M_TARGET_MAIN:
				mMainData.Sampler.SetReadPointer(
					mSubData.Sampler.GetReadPointer());
				break;
			case M_TARGET_SECONDARY:
				mSecondaryData.Sampler.SetReadPointer(
					mMainData.Sampler.GetReadPointer());
				break;
			case M_TARGET_SUB:
				mSubData.Sampler.SetReadPointer(
					mMainData.Sampler.GetReadPointer());
				break;
			}
		}

		if ((temp_message.flags & M_SYNC_MASK) == M_SYNC_OTHER_WITH_THIS)
		{
			switch (temp_message.flags & M_TARGET_MASK)
			{
			case M_TARGET_MAIN:
				mSubData.Sampler.SetReadPointer(
					mMainData.Sampler.GetReadPointer());
				mSecondaryData.Sampler.SetReadPointer(
					mMainData.Sampler.GetReadPointer());
				break;
			case M_TARGET_SECONDARY:
				mMainData.Sampler.SetReadPointer(
					mSecondaryData.Sampler.GetReadPointer());
				mSubData.Sampler.SetReadPointer(
					mSecondaryData.Sampler.GetReadPointer());
				break;
			case M_TARGET_SUB:
				mMainData.Sampler.SetReadPointer(
					mSubData.Sampler.GetReadPointer());
				mSecondaryData.Sampler.SetReadPointer(
					mSubData.Sampler.GetReadPointer());
				break;
			}
		}

		// Data teardown
		if ((temp_message.flags & M_DATA_TEARDOWN_MASK) == M_DATA_TEARDOWN_DELETE)
		{
			delete temp_message.data._void;	// If it works like this
		}
	}
}

void Audio::Music::Manager::Fill(Samples start, Samples sampleCount, float* pData, int channelCount, float* pVoiceData)
{
	int i, j;

	mMainData.Entry->Process(start, sampleCount, pVoiceData, channelCount);
	for (j = 0; j < sampleCount * 2; j++)
	{
		pData[j] += pVoiceData[j];
	}

	mSecondaryData.Entry->Process(start, sampleCount, pVoiceData, channelCount);
	for (j = 0; j < sampleCount * 2; j++)
	{
		pData[j] += pVoiceData[j];
	}

	mSubData.Entry->Process(start, sampleCount, pVoiceData, channelCount);
	for (j = 0; j < sampleCount * 2; j++)
	{
		pData[j] += pVoiceData[j];
	}
	// Save 256 samples for the fourier transform
	long long temp = (long long)sampleCount - (long long)mPreviousSamples.writeAvailable();
	if(temp > 0)
		mPreviousSamples.progressTail(temp);
	float temp_float;
	for (j = 0; j < sampleCount; j++)
	{
		temp_float = (pData[j * 2] + pData[j * 2 + 1]) / 2.0f;
		mPreviousSamples.insert(&temp_float);
	}
}

Audio::Samples Audio::Music::Manager::GetCurrentSampleFromMainMusic()
{
	return mMainData.Sampler.GetReadPointer();
}

void Audio::Music::Manager::GetFrequencies(float* freqArray)
{
	typedef std::complex<float> Complex;
	typedef std::valarray<Complex> CArray;

	// Fetch previous samples
	mPreviousSamples.peakBuff(freqArray, 256);
	// Convert into a complex array
	Complex* test = new Complex[256];
	for (int i = 0; i < 256; i++)
	{
		test[i] = freqArray[i];
	}
	CArray data(test, 256);	// This part can be improved

	// DFT (FFT Cooley-Tukey)
	// The algorithm is based on the DFT formula:
	///// X[k] = SUM<n=0:N-1>(x[n]*exp(-i*2*PI*k*n/N))
	///// X[k] = SUM<n=0:N-1>(x[n]*(cos(2*PI/N)-i*sin(2*PI/N))^(k*n))
	///// X[k] = SUM<n=0:N-1>(x[n]*Twi_N^(k*n))

	// This algorithm speeds up the DFT formula with a
	// divide-and-conquer approach using a "butterfly"
	// summing technique. The technique sorts even and odd
	// buckets which needs to be sorted back using a
	// reverse bit ordering.

	// Variables -----------------
	unsigned int N = data.size();	// Total amount of samples
	unsigned int k = N;				// Help varialbes for the loops
	unsigned int n;					// to achieve the butterfly technique

	// T for Twiddle. This calculates the twiddle constant that will be used
	float thetaT = M_PI / N;
	Complex phiT = Complex(cos(thetaT), -sin(thetaT));
	Complex T;	// Current twiddle

	// FFT calculations
	while (k > 1)
	{
		n = k;
		k >>= 1;
		phiT = phiT * phiT;	// (Twid_N)^k (k*=2)
		T = 1.0L;
		for (unsigned int l = 0; l < k; l++)
		{
			for (unsigned int a = l; a < N; a += n)
			{
				// a and b gets indexes generating the
				// butterfly addition effect
				unsigned int b = a + k;
				Complex t = data[a] - data[b];
				data[a] += data[b];
				data[b] = t * T;
			}
			T *= phiT;		// (Twid_N)^n (n++)
		}
	}

	// Decimate (Reorder using bit reversal)
	unsigned int m = (unsigned int)log2(N);
	for (unsigned int a = 0; a < N; a++)
	{
		unsigned int b = a;
		// Reverse bits
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - m);
		if (b > a)
		{
			Complex t = data[a];
			data[a] = data[b];
			data[b] = t;
		}
	}

	// Normalize and return results
	for (int i = 0; i < 256; i++)
	{
		freqArray[i] = std::abs(data[i]) / 256.f;
	}
	delete[] test;
}

Audio::Music::Manager::Manager()
{
	mMainData.Gain.SetNextPointer(&mMainData.Sampler, true);
	mMainData.Entry = &mMainData.Gain;

	mSecondaryData.Gain.SetNextPointer(&mSecondaryData.Sampler, true);
	mSecondaryData.Entry = &mSecondaryData.Gain;
	
	mSubData.Gain.SetNextPointer(&mSubData.Sampler, true);
	mSubData.Entry = &mSubData.Gain;
}

void Audio::Music::Manager::AddMusicMessage(Music::Message message)
{
	mMusicMessageBuffer.insert(&message);
}
