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

namespace ArenaProperties
{
	constexpr int rows = 12;
	constexpr int columns = 12;
	constexpr float tileRadius = 4.0f;
	static int gridLogic[rows][columns];
}

namespace PlayerProperties
{
	constexpr int numberOfUnits = 3;
	constexpr float unitHealth = 100.0f;
}