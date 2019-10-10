#pragma once
#include <string>
#include "SoundEngine.h"
#include "Mixer.h"
#include "SoundBank.h"

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