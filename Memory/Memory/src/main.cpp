#include "MemoryAPI.h"
#include <iostream>

#include <crtdbg.h>

using namespace std;

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	memory::MemoryManager::Instance();
	cout << endl << endl;
	memory::MemoryManager::End();
	getchar();

	return 0;
}