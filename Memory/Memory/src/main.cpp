#include "MemoryAPI.h"
#include <iostream>

#include <crtdbg.h>

using namespace std;

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	memory::MemoryManager::instance();
	cout << endl << endl;
	memory::MemoryManager::end();
	getchar();

	return 0;
}