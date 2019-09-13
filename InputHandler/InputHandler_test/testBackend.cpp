#include "pch.h"
#include "InputSystems.h"

// Checks if any connection failed
TEST(BackEnd, TestConnection)
{
	WebConnection* tempWebConn = new WebConnection();

	tempWebConn->update();

	EXPECT_TRUE(tempWebConn->isConnected());

	delete tempWebConn;
}

// The backend can get each player's button
TEST(BackEnd, TestWebButtons)
{
	WebConnection* tempWebConn = new WebConnection();

	tempWebConn->update();

	for (size_t i = 0; i < 4; i++)
	{
		EXPECT_TRUE((tempWebConn->getPlayerButton(i) > -1));
	}


	delete tempWebConn;
}

// The backend can get each players tile
TEST(BackEnd, TestWebTiles)
{
	WebConnection* tempWebConn = new WebConnection();

	tempWebConn->update();

	for (size_t i = 0; i < 4; i++)
	{
		EXPECT_TRUE((tempWebConn->getPlayerTile(i, 0) > -1));
		EXPECT_TRUE((tempWebConn->getPlayerTile(i, 1) > -1));
	}


	delete tempWebConn;
}

// The backend ca nget the name
TEST(BackEnd, TestWebNames)
{
	WebConnection* tempWebConn = new WebConnection();

	tempWebConn->update();

	for (size_t i = 0; i < 4; i++)
	{
		EXPECT_TRUE(tempWebConn->getPlayername(i) != "");
	}


	delete tempWebConn;
}


// The input-backend can get each players Button, and anyone of them i pressed
TEST(InputBackend, InputButtons)
{
	InputSystem* backend = new InputSystem();
	EXPECT_TRUE(backend->updateWeb());

	for (size_t i = 0; i < 4; i++)
	{
		bool anyPressed = false;
		EXPECT_TRUE(backend->playerControll[i]->keyU.index > -1);
		if (backend->playerControll[i]->keyU.pressed)
			anyPressed = true;

		EXPECT_TRUE(backend->playerControll[i]->keyD.index > -1);
		if (backend->playerControll[i]->keyD.pressed)
			anyPressed = true;

		EXPECT_TRUE(backend->playerControll[i]->keyL.index > -1);
		if (backend->playerControll[i]->keyL.pressed)
			anyPressed = true;

		EXPECT_TRUE(backend->playerControll[i]->keyR.index > -1);
		if (backend->playerControll[i]->keyR.pressed)
			anyPressed = true;

		EXPECT_TRUE(anyPressed);
	}
	


	delete backend;
}

// The input-backend can get the index of the keyboard
TEST(InputBackend, KeyboardKeyIndex)
{
	InputSystem* backend = new InputSystem();
	EXPECT_TRUE(backend->updateKeyboard());

	EXPECT_TRUE(backend->exitKey->key.index > -1);
	EXPECT_TRUE(backend->ressetKey->key.index > -1);
	EXPECT_TRUE(backend->mouseLKey->key.index > -1);
	EXPECT_TRUE(backend->mouseRKey->key.index > -1);
	EXPECT_TRUE(backend->wsad->keyU.index > -1);
	EXPECT_TRUE(backend->wsad->keyD.index > -1);
	EXPECT_TRUE(backend->wsad->keyL.index > -1);
	EXPECT_TRUE(backend->wsad->keyR.index > -1);

	delete backend;
}


// The input-backend can get each players tile
TEST(InputBackend, InputTiles)
{
	InputSystem* backend = new InputSystem();
	EXPECT_TRUE(backend->updateWeb());

	for (size_t i = 0; i < 4; i++)
	{
		EXPECT_TRUE(backend->players[i]->currButton0 > -1);
		EXPECT_TRUE(backend->players[i]->currButton1 > -1);
	}
	
	delete backend;
}

// The input-backend has all mouse members
TEST(InputBackend, InputMouse)
{
	InputSystem* backend = new InputSystem();
	EXPECT_TRUE(backend->updateMouse());

	EXPECT_TRUE(backend->mouse->oldPos.x > -1);
	EXPECT_TRUE(backend->mouse->oldPos.y > -1);
	EXPECT_TRUE(backend->mouse->newPos.x > -1);
	EXPECT_TRUE(backend->mouse->newPos.y > -1);
	EXPECT_TRUE(backend->mouse->diffFloat2.x > -10000);
	EXPECT_TRUE(backend->mouse->diffFloat2.y > -10000);

	EXPECT_TRUE(backend->mouse->diffLength > -1);

	delete backend;
}
