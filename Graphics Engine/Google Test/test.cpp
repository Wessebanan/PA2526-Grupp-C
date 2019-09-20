#include "pch.h"
#include "../includes/DeviceInterface.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
	#pragma comment(lib, "GraphicsEngine_d.lib")
#else
	#pragma comment(lib, "GraphicsEngine.lib")
#endif // DEBUG


TEST(TestingDeviceInterface, CreationAndDeletion)
{
	graphics::DeviceInterface* pDevice;
	graphics::CreateDeviceInterface(&pDevice);
	EXPECT_TRUE(pDevice);
	graphics::DeleteDeviceInterface(pDevice);
}

void RunTests(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	int testsFailed = RUN_ALL_TESTS();
}

int main(int argc, char** argv)
{
	RunTests(argc, argv);

	if (_CrtDumpMemoryLeaks())
	{
		printf("Memory Leak Detected!\n");
	}

	system("pause");

	return 0;
}