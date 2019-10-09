#include "Voice.h"

Sound::Voice::Voice()
{
	mpEntryPlugin = nullptr;
	mActive = false;
}

Sound::Voice::~Voice()
{
	Delete();
}

void Sound::Voice::Fill(Samples start, Samples count, float* pData)
{
	if (mpEntryPlugin->Process(start, count, pData, 2)
		== Sound::Plugin::Status::STATUS_FINISHED)
	{
		Delete();
	}
}

void Sound::Voice::New(Plugin::Plugin* pEntryPlugin)
{
	Delete();
	mpEntryPlugin = pEntryPlugin;
	mActive = true;
}

bool Sound::Voice::IsActive() const
{
	return mActive;
}

void Sound::Voice::Delete()
{
	if (mpEntryPlugin != nullptr)
	{
		delete mpEntryPlugin;
		mpEntryPlugin = nullptr;
	}
	mActive = false;
}
