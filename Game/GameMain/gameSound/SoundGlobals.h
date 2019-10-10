#pragma once
#include <string>
#include "AudioEngine.h"
#include "AudioMixer.h"
#include "AudioBank.h"

enum SoundFlags
{
	NONE = 0x0,
	REPEAT = 0x1
};

enum SoundName
{
	COIN_TEST_SOUND
};

const std::string SOUND_NAME_PATHS[] =
{
	"coin.wav"
};