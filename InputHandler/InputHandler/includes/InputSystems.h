#pragma once
#ifndef INPUTSYSTEMS_H
#define INPUTSYSTEMS_H

#define ALLKEYS false

#include <DirectXMath.h>
#include <Windows.h>
#include <WinUser.h>


#include "webConnection.h"
#include "ecs.h"

struct key
{
	int index = 0;
	bool pressed = false;
};

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
struct Rotation
{
	DirectX::XMFLOAT2 dir;
};
struct MouseBehavior
{
	DirectX::XMFLOAT2 newPos;
	DirectX::XMFLOAT2 oldPos;
	DirectX::XMFLOAT2 diffFloat2;
	float diffLength = 0.0f;
	bool moved = false;
	bool executed = false;
};

struct WebAction
{
	int currButton0 = 0;
	int currButton1 = 0;

	//WebAction();
};

class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	// returns true if there was a change to any of the allocated keys
	bool updateKeyboard();
	// returns if there was a change to hte mousemovement
	bool updateMouse();
	// returns true, checks the input from the web
	bool updateWeb();

	// Allocated keyboard keys, assinged in constructor
	MovementKeys* wsad = nullptr; // W S A D
	SingleKey* ressetKey = nullptr; // R
	SingleKey* mouseRKey = nullptr; // Rigth mouse
	SingleKey* mouseLKey = nullptr; // Left mouse
	SingleKey* exitKey = nullptr; // ESC

	// Would save the hold keyboard if the software needs to take in more than 3 keys at once
	PBYTE savedState;

	// Holds the mouse change data
	MouseBehavior *mouse = nullptr;

	// The players different buttons
	MovementKeys* playerControll[4];
	// The players tile selected
	WebAction* players[4];

private:
	// Handle for the connection to the website
	WebConnection* webConn = nullptr;
	
	// Saves the whole keyboard
	bool updateSavedState();

	// Calcs and updates the new mouse data
	void modyfiByMouse();

	// Looks and updates if the whole keyboard is saved
	bool checkKeyAll(key *key);
	// Returns true if the key is pressed down
	bool checkKey(key *key);

	// Returns true if the button is the current selected by the user
	bool checkWebKey(int playerIndex, const int button);

	// Reads and updates the selected tiles
	void updateTiles();
	// Reads and updates the buttons
	void updateButtons();
};




#endif // !INPUTSYSTEMS_H