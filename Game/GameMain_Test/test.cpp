#include "pch.h"
#include "Physics/GroundCollision.h"
#include "Physics/MovementLogic.h"
#include "Physics/ObjectCollision.h"
#include "Mesh.h"
#include <crtdbg.h>

int main(int argc, char** argv) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}