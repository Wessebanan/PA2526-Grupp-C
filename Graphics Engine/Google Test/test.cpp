#include "pch.h"
#include "../includes/DeviceInterface.h"

#ifdef _DEBUG
	#pragma comment(lib, "GraphicsEngine_d.lib")
#else
	#pragma comment(lib, "GraphicsEngine.lib")
#endif // DEBUG

namespace GraphicsEngine
{
	TEST(TestingDeviceInterface, CreatingDeviceInterface)
	{
		graphics::DeviceInterface* pDevice;
		graphics::CreateDeviceInterface(&pDevice);
		EXPECT_TRUE(pDevice);
		graphics::DeleteDeviceInterface(pDevice);
	}
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);

	int testValue = RUN_ALL_TESTS();
	system("pause");

	return testValue;
}