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
	tc.position.x = 0.0f;
	tc.position.y = 100.0f;
	tc.position.z = 0.0f;
	tc.scale.x = 0.0f;
	tc.scale.y = 10.0f;
	tc.scale.z = 0.0f;
	tc.rotation.x = 0.0f;
	tc.rotation.y = 0.0f;
	tc.rotation.z = DirectX::XM_PI / 2.0f;
	
	//Create the world matrix given the TransformComponent
	DirectX::XMMATRIX world = UtilityFunctions::GetWorldMatrix(tc);
	//Generate a test position which we will use with the world matrix
	DirectX::XMVECTOR position = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	//Multiply the test position with our world matrix
	position = DirectX::XMVector3Transform(position, world);
	//Store the results in a XMFLOAT3 to get access to the data
	DirectX::XMFLOAT3 posResults;
	XMStoreFloat3(&posResults, position);

    EXPECT_EQ(posResults.x, 20);
	EXPECT_EQ(posResults.y, 50);
	EXPECT_EQ(posResults.z, 100);
}

TEST(CameraFunctions, InitDevCamera) {
	ecs::EntityComponentSystem mEcs;
	CameraFunctions::CreateDevCamera(mEcs);
		
	int numberOfTc = mEcs.getComponentCountOfType(ecs::components::TransformComponent::typeID);
	int numberOfCc = mEcs.getComponentCountOfType(ecs::components::CameraComponent::typeID);

	EXPECT_EQ(numberOfCc + numberOfTc, 2);
	EXPECT_EQ(numberOfCc, 1);
	EXPECT_EQ(numberOfTc, 1);
}

TEST(CameraFunctions, CreateCameraSystems) {
	ecs::EntityComponentSystem mEcs;
	CameraFunctions::CreateCameraSystems(mEcs);

	int nrOfSystems = mEcs.getTotalSystemCount();
	EXPECT_EQ(nrOfSystems, 1);
}