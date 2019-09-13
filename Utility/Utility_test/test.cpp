#include "pch.h"
#include <CameraFunctions.h>
#include <UtilityFunctions.h>

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(UtilityFunctions, GetWorldMatrix) {
	ecs::components::TransformComponent tc;
	tc.position.x = 10.0f;
	tc.position.y = 10.0f;
	tc.position.z = 10.0f;
	tc.scale.x = 10.0f;
	tc.scale.y = 20.0f;
	tc.scale.z = 30.0f;
	
	DirectX::XMMATRIX world = UtilityFunctions::GetWorldMatrix(tc);
	DirectX::XMVECTOR position = DirectX::XMVectorSet(1.0f, 2.0f, 3.0f, 1.0f);
	position = DirectX::XMVector3Transform(position, world);
	DirectX::XMFLOAT3 posResults;
	XMStoreFloat3(&posResults, position);

    EXPECT_EQ(posResults.x, 20);
	EXPECT_EQ(posResults.y, 50);
	EXPECT_EQ(posResults.z, 100);
}

TEST(CameraFunctions, InitDevCamera) {
	ecs::EntityComponentSystem mEcs;
	CameraFunctions::InitDevCamera(mEcs);
}

TEST(CameraFunctions, InitCameraSystems) {
	ecs::EntityComponentSystem mEcs;
	CameraFunctions::InitCameraSystems(mEcs);
}