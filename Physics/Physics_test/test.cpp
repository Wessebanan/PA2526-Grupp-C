#include "gtest/gtest.h"
//#include "GroundCollision.h"
//#include "MovementLogic.h"
//#include "ObjectCollision.h"
#include "BoundingVolumeTests.h"
#include <crtdbg.h>

int main(int argc, char** argv) {
	
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}