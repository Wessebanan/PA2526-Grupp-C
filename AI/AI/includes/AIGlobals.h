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
	NONE, MOVE, IDLE, ATTACK, LOOT, FLEE, PATHFINDING
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
	SQUARE, // for spawning on thier own part of the map
	NOHOLMES // without them can change name later if needed
};

struct Color
{
	unsigned char r, g, b;
	Color() : r(0), g(0), b(0) {}
	Color(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
};

// Defining some useful colors.
#define RED				Color(117, 1, 1)
#define BRIGHT_RED		Color(167, 51, 51)
#define PURPLE			Color(74, 1, 117)
#define BRIGHT_PURPLE	Color(124, 51, 167)
#define BLUE			Color(47, 62, 236)
#define BRIGHT_BLUE		Color(97, 112, 255)
#define GREEN			Color(0, 93, 5)
#define BRIGHT_GREEN	Color(50, 143, 55)
#define WHITE			Color(255, 255, 255)
#define BLACK			Color(0, 0, 0)

// Defining player colors as current player colors.
#define PLAYER1_COLOR RED
#define PLAYER2_COLOR PURPLE
#define PLAYER3_COLOR BLUE
#define PLAYER4_COLOR GREEN