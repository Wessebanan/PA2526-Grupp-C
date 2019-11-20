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

	GAME_OBJECT_TYPE_WEAPON_OFFSET_TAG,
	GAME_OBJECT_TYPE_WEAPON_SWORD,			// All weapons with meshes first
	GAME_OBJECT_TYPE_WEAPON_HAMMER,
	GAME_OBJECT_TYPE_WEAPON_BOMB,

	GAME_OBJECT_TYPE_WEAPON_PROJECTILE,
	GAME_OBJECT_TYPE_WEAPON_FIST,

	/*
		Traps
	*/

	GAME_OBJECT_TYPE_TRAP_OFFSET_TAG,
	GAME_OBJECT_TYPE_TRAP_FIRE,
	GAME_OBJECT_TYPE_TRAP_FREEZE,
	GAME_OBJECT_TYPE_TRAP_SPRING,

	/* 
		Meshes 
	*/

	GAME_OBJECT_TYPE_BARREL_STONES,
	GAME_OBJECT_TYPE_BARREL_BARREL,

	// -- Insert New Meshes Directly Below --

	GAME_OBJECT_TYPE_PINE_LEAVES,
	GAME_OBJECT_TYPE_PINE_TRUNK,

	GAME_OBJECT_TYPE_TOWER_TOWER,
	GAME_OBJECT_TYPE_TOWER_CAGE,

	GAME_OBJECT_TYPE_DESERT_CACTUS,
	GAME_OBJECT_TYPE_DESERT_BOX,
	GAME_OBJECT_TYPE_DESERT_SKULL,

	GAME_OBJECT_TYPE_MESH_GIANTSKULL,

	GAME_OBJECT_TYPE_MESH_CAGE,

	GAME_OBJECT_TYPE_MESH_COWSKULL,

	GAME_OBJECT_TYPE_MESH_BOX,

	GAME_OBJECT_TYPE_TREE_LEAVES,
	GAME_OBJECT_TYPE_TREE_TRUNK,
	GAME_OBJECT_TYPE_TREE_ROCK,

	// End

	GAME_OBJECT_TYPE_MESH_ERROR,

	GAME_OBJECT_TYPE_MESH_END,

	/*
		Misc
	*/

	GAME_OBJECT_TYPE_QUAD,
};

constexpr int OFFSET_TRAP = 3;

constexpr int MESH_TYPE_COUNT = 12;
constexpr int SCENE_OBJECT_TYPE_COUNT = GAME_OBJECT_TYPE_WINTERTREE - GAME_OBJECT_TYPE_SCENE_OBJECT;
constexpr int TRAP_TYPE_COUNT = 3; // Number of different trap types to be rendered
constexpr int WEAPON_TYPE_COUNT = 3; // Number of different weapon types to be rendered

constexpr int SCENE_OBJECT_ENUM_OFFSET = GAME_OBJECT_TYPE_SCENE_OBJECT + 1;

/* MESH QUICK VALUES */
constexpr int GAME_OBJECT_TYPE_MESH_START = GAME_OBJECT_TYPE_BARREL_STONES;
constexpr int GAME_OBJECT_TYPE_MESH_COUNT = GAME_OBJECT_TYPE_MESH_END - GAME_OBJECT_TYPE_BARREL_STONES;
