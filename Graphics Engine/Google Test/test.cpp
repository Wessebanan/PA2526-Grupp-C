#include "pch.h"
#include "../includes/DeviceInterface.h"

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