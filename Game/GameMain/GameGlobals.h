#pragma once

using GAME_OBJECT_TYPE = unsigned int;
enum GAME_OBJECT_TYPES
{
	/*
		Scenery
	*/

	GAME_OBJECT_TYPE_TILE,

	GAME_OBJECT_TYPE_SCENE_OBJECT,
	GAME_OBJECT_TYPE_BARREL,
	GAME_OBJECT_TYPE_BOX,
	GAME_OBJECT_TYPE_CACTUS,

	GAME_OBJECT_TYPE_CAGE,
	GAME_OBJECT_TYPE_COWSKULL,
	GAME_OBJECT_TYPE_FRUITTREE,

	GAME_OBJECT_TYPE_GIANTSKULL,
	GAME_OBJECT_TYPE_TOWER,
	GAME_OBJECT_TYPE_WINTERTREE,

	/*
		Living things
	*/

	GAME_OBJECT_TYPE_UNIT,

	/*
		Weapons
	*/

	GAME_OBJECT_TYPE_SWORD,
	GAME_OBJECT_TYPE_PROJECTILE,
	GAME_OBJECT_TYPE_FIST
};

constexpr int MESH_TYPE_COUNT = 12;
constexpr int SCENE_OBJECT_COUNT = 9;

constexpr int SCENE_OBJECT_ENUM_OFFSET = GAME_OBJECT_TYPE_SCENE_OBJECT + 1;