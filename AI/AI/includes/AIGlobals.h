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