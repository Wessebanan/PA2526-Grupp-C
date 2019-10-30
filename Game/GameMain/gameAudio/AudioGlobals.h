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
	CC_TEST_CALM_SONG,
	SILENCE_SOUND,
	TRUMPET_FANFARE_SOUND,
	GRUNT_HURT_1_SOUND,
	GRUNT_HURT_2_SOUND,
	PLOP_1_SOUND,
	PLOP_2_SOUND,
	PUNCH_SOUND,
	SCREAM_SOUND,
	SWISH_SOUND,
	SWORD_CLANG_SOUND
};

const std::string AUDIO_NAME_PATHS[] =
{
	"../AudioFiles/coin.wav",
	"../AudioFiles/cc_song.wav",
	"../AudioFiles/cc_drums.wav",
	"../AudioFiles/cc_chill.wav",
	"../AudioFiles/silence.wav",
	"../AudioFiles/fanfare.wav",
	"../AudioFiles/grunt1.wav",
	"../AudioFiles/grunt2.wav",
	"../AudioFiles/plop1.wav",
	"../AudioFiles/plop2.wav",
	"../AudioFiles/punch1.wav",
	"../AudioFiles/scream.wav",
	"../AudioFiles/swish.wav",
	"../AudioFiles/sword.wav",
};

