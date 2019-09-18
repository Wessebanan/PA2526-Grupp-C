// PingingWebsite.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//From: https://stackoverflow.com/questions/32205981/reading-json-files-in-c

//#include "InitInputHandler.h"

#include <thread>
#include <chrono>



#include "webConnection.h"

 
int main()
{

	WebConnection conn;

	int temp = 0;
	while (conn.isConnected() || temp < 10)
	{
		++temp;
		this_thread::sleep_for(chrono::seconds(1));
		for (size_t i = 0; i < conn.getNrOfPlayers(); i++)
		{
			cout << "Player " << i << ": " << conn.getPlayername(i);
			cout << " has selected tile (" << conn.getPlayerTile(i, 0);
			cout << ", " << conn.getPlayerTile(i, 1);
			cout << ") and selected button " << conn.getPlayerButton(i);
			cout << endl;
		}
	}

	//conn.update();
		
	return 0;
}