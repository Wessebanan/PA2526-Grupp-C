// PingingWebsite.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//From: https://stackoverflow.com/questions/32205981/reading-json-files-in-c
#include <thread>
#include <chrono>


//#include "ecs.h"
//#include "HandleInputBackend.h"

#include "InputBackend.h"
 
int main()
{
	InputBackend* inp = new InputBackend;


	//InputBackend inpSys;
	
	
	while (true)
	{
		inp->updateKeyboard();
		inp->updateMouse();
		inp->updateWeb();

		//cout << "mouse: x = " << inp->mpMouse->mDiffFloat2.x;
		//cout << "| y = " << inp->mpMouse->mDiffFloat2.y << endl;

		//this_thread::sleep_for(chrono::seconds(1));
	}

	////WebConnection conn;

	//int temp = 0;
	//while (conn.isConnected() || temp < 10)
	//{
	//	++temp;
	//	this_thread::sleep_for(chrono::seconds(5));
	//	for (size_t i = 0; i < conn.getNrOfPlayers(); i++)
	//	{
	//		cout << "Player " << i << ": " << conn.getPlayername(i);
	//		cout << " has selected tile (" << conn.getPlayerTile(i, 0);
	//		cout << ", " << conn.getPlayerTile(i, 1);
	//		cout << ") and selected button " << conn.getPlayerButton(i);
	//		cout << endl;
	//	}
	//}

	//conn.update();
		
	return 0;
}