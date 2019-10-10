#include "AudioVoice.h"

Audio::Voice::Voice()
{
	mpEntryPlugin = nullptr;
	mActive = false;
}

Audio::Voice::~Voice()
{
	Delete();
}

void Audio::Voice::Fill(Samples start, Samples count, float* pData)
{
	if (mpEntryPlugin->Process(start, count, pData, 2)
		== Audio::Plugin::Status::STATUS_FINISHED)
	{
		Delete();
	}
}

void Audio::Voice::New(Plugin::Plugin* pEntryPlugin)
{
	Delete();
	mpEntryPlugin = pEntryPlugin;
	mActive = true;
}

bool Audio::Voice::IsActive() const
{
	return mActive;
}

void Audio::Voice::Delete()
{
	if (mpEntryPlugin != nullptr)
	{
		delete mpEntryPlugin;
		mpEntryPlugin = nullptr;
	}
	mActive = false;
}
