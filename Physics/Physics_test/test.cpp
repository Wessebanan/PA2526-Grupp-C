#include "pch.h"
//#include "MovementLogic.h"
//#include "GroundCollision.h"
#include <crtdbg.h>
#include "Mesh.h"

TEST(MeshTest, MessAroundWithMesh)
{
	ModelLoader::Mesh mesh;
	//mesh.LoadFBX("../TestModel/untitled2.fbx");
	EXPECT_TRUE(mesh.HasSkeleton());
}

int main(int argc, char** argv) {
	
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}