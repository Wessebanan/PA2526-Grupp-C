#pragma once
#include <DirectXMath.h>
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <WinUser.h>


#include "webConnection.h"
#include "ecs.h"

// combines the key with the index and if it is pressed
struct key
{
	int index = 0;
	bool pressed = false;
};

// Holds the up, down, left and rigth keys
struct MovementKeys
{
	key keyU;

	key keyD;

	key keyL;

	key keyR;

	MovementKeys(
		const char up,
		const char down,
		const char left,
		const char rigth)
	{
		keyU.index = up;
		keyD.index = down;
		keyL.index = left;
		keyR.index = rigth;
	};
};

// Holds one key
struct SingleKey
{
	key key;
	
	SingleKey(
		const int setKey
	)
	{
		key.index = setKey;
	}
};
// Holds the varibles that calculates the mouse movment
struct MouseBehavior
{
	// The new pos of the mouse this frame
	DirectX::XMFLOAT2 mNewPos;
	// the pos the last frame
	DirectX::XMFLOAT2 mOldPos;
	// A vector describing the movment since last frame
	DirectX::XMFLOAT2 mDiffFloat2;
	// the length of hte diff vector
	float mDiffLength = 0.0f;
};

// The tile the user has selected
struct WebTile
{
	// -1 if none selected
	int mCordX = -1;
	// -1 if none selected
	int mCordY = -1;
};
// The last button pressed
struct WebButton
{
	// -1 if none selected yet
	int mButton = -1;
};
// The last command selected by the user
struct WebCommand
{
	// holds the command, is NaN if none has been selected yet
	string mCommand = "NaN";
};

class InputBackend
{
public:
	InputBackend();
	~InputBackend();

	// returns true if there was a change to any of the allocated keys
	bool updateKeyboard();
	// returns if there was a change to hte mousemovement
	bool updateMouse();
	// returns true, checks the input from the web
	bool updateWeb();

	// Allocated keyboard keys, assinged in constructor
	MovementKeys* mpWsad = nullptr; // W S A D
	SingleKey* mpRessetKey = nullptr; // R
	SingleKey* mpMouseRKey = nullptr; // Rigth mouse
	SingleKey* mpMouseLKey = nullptr; // Left mouse
	SingleKey* mpExitKey = nullptr; // ESC

	// Holds the mouse change data
	MouseBehavior* mpMouse = nullptr;

	// The players different buttons
	WebButton* mpUserButton[4];
	// The players tile selected
	WebTile* mpUserTile[4];
	// The user command selected
	WebCommand* mpUserCommand[4];

private:
	// Handle for the connection to the website
	WebConnection* mpWebConn = nullptr;

	// Calcs and updates the new mouse data
	void modyfiByMouse();

	// Returns true if the key is pressed down
	bool checkKey(key *key);

	// Reads and updates the selected tiles for all users
	void updateTiles();
	// Reads and updates the buttons for all users
	void updateButtons();
	// Reads and updates the commands for all users
	void updateCommands();
	// not implemented but webb has it stored
	//void updateName();
};