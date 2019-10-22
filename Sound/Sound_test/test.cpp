#include "pch.h"
#include "TestHelpers.hpp"
//#include "TestSuite1.hpp"
#include "TestSuite2.hpp"

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

// MOST TEST REQUIRES HEADPHONES OR LOUDSPEAKER TO BE PLUGGED IN!!!