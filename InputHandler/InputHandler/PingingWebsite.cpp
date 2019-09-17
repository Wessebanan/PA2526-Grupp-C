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
	while (true)
	{
		//cout << ++temp;
		this_thread::sleep_for(chrono::seconds(1));
		//conn.getPlayername(0);
	}

	//conn.update();
		
	return 0;
}