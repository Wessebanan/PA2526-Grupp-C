#pragma once

#define ARENA_ROWS 12
#define ARENA_COLUMNS 12
#define TILE_RADIUS 4

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
	MOVE, IDLE, ATTACK, LOOT
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
