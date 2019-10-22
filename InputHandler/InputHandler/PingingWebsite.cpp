// PingingWebsite.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//From: https://stackoverflow.com/questions/32205981/reading-json-files-in-c
#include <thread>
#include <chrono>


#include "InputBackend.h"
 
int main()
{
	InputBackend* inp = new InputBackend;
	
	while (true)
	{
		inp->updateKeyboard();
		inp->updateMouse();
		inp->updateWeb();

		cout << "mouse: x = " << inp->mpMouse->mDiffFloat2.x;
		cout << "| y = " << inp->mpMouse->mDiffFloat2.y << endl;

		this_thread::sleep_for(chrono::seconds(1));
	}
		
	return 0;
}