#include "pch.h"
#include "InputSystems.h"

InputSystem* backend;

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