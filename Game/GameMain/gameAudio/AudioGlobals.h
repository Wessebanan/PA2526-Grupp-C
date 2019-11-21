#pragma once
#include <string>
#include "AudioEngine.h"
#include "AudioMixer.h"
#include "AudioBank.h"

#define SOUND_COOLDOWN 0.7f

enum SoundFlags
{
	SF_NONE = 0x0,
	SF_REPEAT = 0x1
};

enum MusicFlags
{
	MF_NONE = 0x0,
	MF_REPLACE = 0x1
};

#define SOUND_FOLDER "../AudioFiles/"

#define ADD_SOUND(x) SOUND_##x,
enum AudioName
{
	#include "SoundIncludes"
	SOUND_COUNT
};
#undef ADD_SOUND

#define ADD_SOUND(x) SOUND_FOLDER #x ".wav",
const std::string AUDIO_NAME_PATHS[] =
{
	#include "SoundIncludes"
	SOUND_FOLDER "coin.wav"
};
#undef ADD_SOUND