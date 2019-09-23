#pragma once
#include "Mesh.h"

namespace GroundCollision
{
	TEST(GroundCollision, FindLowestPoint)
	{
		ModelLoader::Mesh m;
		EXPECT_HRESULT_SUCCEEDED(m.LoadFBX("../TestModel/deer.fbx"));
	
	}
} // GroundCollision