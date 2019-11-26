#pragma once

#define MAX_ARENA_ROWS 18
#define MAX_ARENA_COLUMNS 18
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
	NONE, MOVE, IDLE, ATTACK, LOOT, FLEE, PATHFINDING, RALLY
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
	unsigned char r, g, b, a;
	Color() : r(0), g(0), b(0), a(0) {}
	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 1) : r(r), g(g), b(b), a(a) {}
};

// Defining some useful colors.
// Alpha value is currently being used to determine outline colors in the outline pass, pls no touch
#define RED				Color(117, 1, 1, 1)
#define PURPLE			Color(74, 1, 117, 5)
#define BLUE			Color(47, 62, 236, 26)
#define GREEN			Color(0, 93, 5, 105)      

#define WHITE			Color(255, 255, 255)
#define BLACK			Color(0, 0, 0)

#define BRIGHT_FACTOR	100
#define BRIGHT_RED		Color(117 + BRIGHT_FACTOR, 1 + BRIGHT_FACTOR, 1 + BRIGHT_FACTOR)
#define BRIGHT_PURPLE	Color(74 + BRIGHT_FACTOR, 1 + BRIGHT_FACTOR, 117 + BRIGHT_FACTOR)
#define BRIGHT_BLUE		Color(47 + BRIGHT_FACTOR, 62 + BRIGHT_FACTOR, 236)
#define BRIGHT_GREEN	Color(0 + BRIGHT_FACTOR, 93 + BRIGHT_FACTOR, 5 + BRIGHT_FACTOR)


// Defining player colors as current player colors.
#define PLAYER1_COLOR RED
#define PLAYER2_COLOR PURPLE
#define PLAYER3_COLOR BLUE
#define PLAYER4_COLOR GREEN