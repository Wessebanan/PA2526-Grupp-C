#pragma once

#define MAX_ARENA_ROWS 30
#define MAX_ARENA_COLUMNS 30
#define TILE_RADIUS 1

using TileType = unsigned int;
enum TileTypes //enum used in AIcomponents right now 
{
	GAME_FIELD,
	WATER,

	TILE_TYPE_COUNT,
	UNDEFINED
};

enum STATE
{
	MOVE, IDLE, ATTACK, LOOT, FLEE
};

enum PLAYER
{
	PLAYER1, PLAYER2, PLAYER3, PLAYER4
};

struct int2
{
	int x, y;
	int2(int a = 0, int b = 0) { x = a; y = b; }
};

namespace PlayerProperties
{
	constexpr int numberOfUnits = 3;
	constexpr float unitHealth = 100.0f;
}

// Declares in what way the map 
enum MAPINITSETTING
{
	HOLMES, // Small islands on each side
	NOHOLMES // without them can change name later if needed
};