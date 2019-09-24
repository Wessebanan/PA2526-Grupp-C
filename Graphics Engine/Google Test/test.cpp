#include "pch.h"
#include "../includes/DeviceInterface.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "TestingDeviceInterface.h"
#include "TestingRenderer.h"

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	::testing::InitGoogleTest(&argc, argv);
	int testsFailed = RUN_ALL_TESTS();

	system("pause");

	return 0;
}