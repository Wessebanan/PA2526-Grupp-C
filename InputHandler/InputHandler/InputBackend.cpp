#include "InputBackend.h"

InputBackend::InputBackend()
{
	//WEB
	this->webConn = new WebConnection();

	//KEYBOARD
	this->wsad = new MovementKeys('W', 'S', 'A', 'D');
	this->ressetKey = new SingleKey((int)'R'); //0x52
	this->mouseRKey = new SingleKey(0x02);
	this->mouseLKey = new SingleKey(0x01);
	this->exitKey = new SingleKey((int)VK_ESCAPE);

	//MOUSE
	this->mouse = new MouseBehavior();

	// PLAYER ACTION
	this->players[0] = new WebAction();
	this->players[1] = new WebAction();
	this->players[2] = new WebAction();
	this->players[3] = new WebAction();

	// WEB CONTROLL
	this->playerControll[0] = new MovementKeys('0', '1', '2', '3');
	this->playerControll[1] = new MovementKeys('0', '1', '2', '3');
	this->playerControll[2] = new MovementKeys('0', '1', '2', '3');
	this->playerControll[3] = new MovementKeys('0', '1', '2', '3');
}

InputBackend::~InputBackend()
{
	delete this->wsad;
	delete this->ressetKey;
	delete this->mouseRKey;
	delete this->mouseLKey;
	delete this->exitKey;

	delete mouse;

	for (size_t i = 0; i < 4; i++)
	{
		delete playerControll[i];
		delete players[i];
	}

	delete webConn;
}

bool InputBackend::updateKeyboard()
{
	bool retVal = false;
				  
	if (this->checkKey(&this->wsad->keyU))
		retVal = true;
	if (this->checkKey(&this->wsad->keyD))
		retVal = true;
	if (this->checkKey(&this->wsad->keyL))
		retVal = true;
	if (this->checkKey(&this->wsad->keyR))
		retVal = true;


	if (this->checkKey(&this->ressetKey->key))
		retVal = true;
	if (this->checkKey(&this->mouseRKey->key))
		retVal = true;
	if (this->checkKey(&this->mouseLKey->key))
		retVal = true;
	if (this->checkKey(&this->exitKey->key))
		retVal = true;
	


	return retVal; 
}

void InputBackend::modyfiByMouse()
{
	DirectX::XMFLOAT2 diff(
		  this->mouse->newPos.x - this->mouse->oldPos.x
		, this->mouse->newPos.y - this->mouse->oldPos.y
	);

	this->mouse->diffLength = sqrt((diff.x * diff.x) + (diff.y * diff.y));

	this->mouse->diffFloat2 = diff;

	this->mouse->oldPos = this->mouse->newPos;
}

bool InputBackend::updateMouse()
{
	bool retVal = false;
	POINT temp;

	GetCursorPos(&temp);

	if (this->mouse->oldPos.x != temp.x || this->mouse->oldPos.y != temp.y)
	{
		retVal = true;
		this->mouse->newPos.x = temp.x;
		this->mouse->newPos.y = temp.y;

		this->modyfiByMouse();
	}
	else
	{
		this->mouse->diffFloat2.x = 0.0f;
		this->mouse->diffFloat2.y = 0.0f;
		this->mouse->diffLength = 0.0f;
	}
	return retVal;
}

bool InputBackend::updateWeb()
{
	this->updateTiles();
	this->updateButtons();
	//this->updateName();

	return true;
}

bool InputBackend::checkKey(key *key)
{
	bool retVal = false;
	 
	if (GetAsyncKeyState(key->index))
	{
		if (!key->pressed)
		{
			retVal = true;
			key->pressed = true;
		}
	}
	else if(key->pressed)
	{
		retVal = true;
		key->pressed = false;
	}

	return retVal;
}

bool InputBackend::checkWebKey(int playerIndex, const int button)
{
	bool retVal = (this->webConn->getPlayerButton(playerIndex) == button);
	return retVal;
}

void InputBackend::updateTiles()
{
	// Checks and updates each players UDLR movment keys
	for (size_t playerIndex = 0; playerIndex < 4; playerIndex++)
	{
		this->players[playerIndex]->currButton0 = this->webConn->getPlayerTile(playerIndex, 0);
		this->players[playerIndex]->currButton1 = this->webConn->getPlayerTile(playerIndex, 1);
	}
}

void InputBackend::updateButtons()
{
	// Checks and updates each players UDLR movment keys
	for (size_t playerIndex = 0; playerIndex < 4; playerIndex++)
	{
		if (checkWebKey(playerIndex, 0))
		{
			this->playerControll[playerIndex]->keyU.pressed = true;

		}
		else
		{
			this->playerControll[playerIndex]->keyU.pressed = false;
		}
		
		if (checkWebKey(playerIndex, 1))
		{
			this->playerControll[playerIndex]->keyD.pressed = true;

		}
		else
		{
			this->playerControll[playerIndex]->keyD.pressed = false;
		}
		
		if (checkWebKey(playerIndex, 2))
		{
			this->playerControll[playerIndex]->keyL.pressed = true;

		}
		else
		{
			this->playerControll[playerIndex]->keyL.pressed = false;
		}

		if (checkWebKey(playerIndex, 3))
		{
			this->playerControll[playerIndex]->keyR.pressed = true;

		}
		else
		{
			this->playerControll[playerIndex]->keyR.pressed = false;
		}
	}
}

