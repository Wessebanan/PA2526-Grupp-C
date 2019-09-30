#pragma once

using TileType = unsigned int;
enum TileTypes //enum used in AIcomponents right now 
{
	GRASS,
	STONE,
	WATER,

	TILETYPECOUNT,
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

struct TileData
{
	int entityID = 1;
	bool isPassable;
};

struct TileTEST
{
	bool gridPassable[12][12];
	unsigned int gridID[12][12];
};

struct int2
{
	int x; 
	int y;
};

namespace ArenaProperties
{
	constexpr int rows = 12;
	constexpr int columns = 12;
	constexpr float tileRadius = 4.0f;
	//static TileData gridLogic[rows][columns];
	static bool gridID[rows][columns];
	static unsigned int MackeID[rows][columns];
	static bool gridPassable[rows][columns];
	static int test;

//	static TileTEST tileTest;
}

namespace PlayerProperties
{
	constexpr int numberOfUnits = 3;
	constexpr float unitHealth = 100.0f;
}