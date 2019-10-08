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

void Sound::Voice::Fill(Samples Start, Samples Count, float* Data)
{
	mpEntryPlugin->Process(Start, Count, Data, 2);
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
}
