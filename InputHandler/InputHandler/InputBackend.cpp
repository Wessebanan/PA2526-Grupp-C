#include "InputBackend.h"

InputBackend::InputBackend()
{
	//WEB
	mpWebConn = new WebConnection();

	//KEYBOARD
	mpWsad = new MovementKeys('W', 'S', 'A', 'D');
	mpRessetKey = new SingleKey((int)'R'); //0x52
	mpMouseRKey = new SingleKey(0x02);
	mpMouseLKey = new SingleKey(0x01);
	mpExitKey = new SingleKey((int)VK_ESCAPE);

	//MOUSE
	mpMouse = new MouseBehavior();

	// USER	INPUT FOR WEB
	for (size_t i = 0; i < 4; i++)
	{
		mpUserButton[i] = new WebButton();
		mpUserTile[i] = new WebTile();
		mpUserCommand[i] = new WebCommand();
	}
}

InputBackend::~InputBackend()
{
	delete mpWsad;
	delete mpRessetKey;
	delete mpMouseRKey;
	delete mpMouseLKey;
	delete mpExitKey;

	delete mpMouse;

	for (size_t i = 0; i < 4; i++)
	{
		delete mpUserButton[i];
		delete mpUserTile[i];
		delete mpUserCommand[i];
	}

	delete mpWebConn;
}

bool InputBackend::updateKeyboard()
{
	bool ret_val = false;
				  
	// goes over all keys

	if (this->checkKey(&mpWsad->keyU))
		ret_val = true;
	if (this->checkKey(&mpWsad->keyD))
		ret_val = true;
	if (this->checkKey(&mpWsad->keyL))
		ret_val = true;
	if (this->checkKey(&mpWsad->keyR))
		ret_val = true;


	if (this->checkKey(&mpRessetKey->key))
		ret_val = true;
	if (this->checkKey(&mpMouseRKey->key))
		ret_val = true;
	if (this->checkKey(&mpMouseLKey->key))
		ret_val = true;
	if (this->checkKey(&mpExitKey->key))
		ret_val = true;
	


	return ret_val; 
}

void InputBackend::modyfiByMouse()
{
//	DirectX::XMFLOAT2 diff(
//		  mpMouse->mNewPos.x - mpMouse->mOldPos.x
//		, mpMouse->mNewPos.y - mpMouse->mOldPos.y
//	);
//
//	mpMouse->mDiffLength = sqrt((diff.x * diff.x) + (diff.y * diff.y));
//
//	mpMouse->mDiffFloat2 = diff;
//
//	mpMouse->mOldPos = mpMouse->mNewPos;
}

bool InputBackend::updateMouse()
{
	bool ret_val = false;
	POINT temp;

	GetCursorPos(&temp);

	//// checks if it any of the cords was changed
	//if (mpMouse->mOldPos.x != temp.x || mpMouse->mOldPos.y != temp.y)
	//{
	//	// saves and updates members
	//	ret_val = true;
	//	//mpMouse->mNewPos.x = temp.x;
	//	//mpMouse->mNewPos.y = temp.y;

	//	this->modyfiByMouse();
	//}
	//else // else resets 
	//{
	//	//mpMouse->mDiffFloat2.x = 0.0f;
	//	//mpMouse->mDiffFloat2.y = 0.0f;
	//	//mpMouse->mDiffLength = 0.0f;
	//}
	return ret_val;
}

bool InputBackend::updateWeb()
{
	this->updateTiles();
	this->updateButtons();
	this->updateCommands();
	//this->updateName();

	return true;
}

bool InputBackend::checkKey(key *key)
{
	bool ret_val = false;
	 
	// Checks if the index has been pressed sicnce the last time this was run
	if (GetAsyncKeyState(key->index))
	{
		// makes it so that it wont jump back and forth
		if (!key->pressed)
		{
			ret_val = true;
			key->pressed = true;
		}
	}
	else if(key->pressed)
	{
		ret_val = true;
		key->pressed = false;
	}

	return ret_val;
}

void InputBackend::updateTiles()
{
	// Checks and updates each players cords
	for (size_t playerIndex = 0; playerIndex < 4; playerIndex++)
	{
		mpUserTile[playerIndex]->mCordX = mpWebConn->GetUserTile(playerIndex, 0);
		mpUserTile[playerIndex]->mCordY = mpWebConn->GetUserTile(playerIndex, 1);
	}
}

void InputBackend::updateButtons()
{
	// Checks and updates each players UDLR movment keys
	for (size_t playerIndex = 0; playerIndex < 4; playerIndex++)
	{
		mpUserButton[playerIndex]->mButton = mpWebConn->GetUserButton(playerIndex);
	}
}

void InputBackend::updateCommands()
{
	// Checks and updates each players current command
	for (size_t playerIndex = 0; playerIndex < 4; playerIndex++)
	{
		mpUserCommand[playerIndex]->mCommand = mpWebConn->GetUserCommand(playerIndex);
	}
}

