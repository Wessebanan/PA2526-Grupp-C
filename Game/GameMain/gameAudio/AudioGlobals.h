#pragma once
#include <string>
#include "AudioEngine.h"
#include "AudioMixer.h"
#include "AudioBank.h"

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

enum AudioName
{
	COIN_TEST_SOUND,
	CC_TEST_SONG,
	CC_TEST_SONG_DRUMS,
	TRUMPET_FANFARE,
	GRUNT_1,
	GRUNT_2,
	PLOP,
	DEATH_SCREAM,
	SWISH,
	SWORD_HIT
};

const std::string AUDIO_NAME_PATHS[] =
{
	"../AudioFiles/coin.wav",
	"../AudioFiles/cc_song.wav",
	"../AudioFiles/cc_drums.wav",
	"../AudioFiles/fanfare.wav",
	"../AudioFiles/grunt1.wav",
	"../AudioFiles/grunt2.wav",
	"../AudioFiles/plop.wav",
	"../AudioFiles/scream.wav",
	"../AudioFiles/swish.wav",
	"../AudioFiles/sword.wav"
};

