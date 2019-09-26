#include "pch.h"
#include "CameraSystems.h"
#include "ecs.h"
#include "CameraFunctions.h"
#include "UtilityFunctions.h"
#include "DebugInfoTestHeader.h"

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(UtilityFunctions, GetWorldMatrix) {
	ecs::components::TransformComponent tc;
	tc.position.x = 0.0f;
	tc.position.y = -2.0f;
	tc.position.z = 0.0f;
	tc.scale.x = 3.0f;
	tc.scale.y = 2.0f;
	tc.scale.z = 1.0f;
	tc.rotation.x = 0.0f;
	tc.rotation.y = 0.0f;
	tc.rotation.z = DirectX::XM_PI / 2.0f;

	//Create the world matrix given the TransformComponent
	DirectX::XMMATRIX world = UtilityFunctions::GetWorldMatrix(tc);
	//Generate a test position which we will use with the world matrix
	DirectX::XMVECTOR position = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	//Multiply the test position with our world matrix
	position = DirectX::XMVector3Transform(position, world);
	//Store the results in a XMFLOAT3 to get access to the data
	DirectX::XMFLOAT3 posResults;
	XMStoreFloat3(&posResults, position);

	//Because of inprecision, we're checking coordinates within
	//a small range of the expected outcome
	EXPECT_TRUE((posResults.x < -1.999f) && (posResults.x > -2.001f));
	EXPECT_TRUE((posResults.y > 0.999f) && (posResults.y < 1.001f));
	EXPECT_TRUE((posResults.z > 0.999f) && (posResults.z < 1.001f));
}

TEST(CameraFunctions, CreateDevCamera) {
	ecs::EntityComponentSystem mEcs;
	CameraFunctions::CreateDevCamera(mEcs);

	int numberOfTc = mEcs.getComponentCountOfType(ecs::components::TransformComponent::typeID);
	int numberOfCc = mEcs.getComponentCountOfType(ecs::components::CameraComponent::typeID);

	EXPECT_EQ(numberOfCc + numberOfTc, 2);
	EXPECT_EQ(numberOfCc, 1);
	EXPECT_EQ(numberOfTc, 1);
}

TEST(CameraFunctions, MoveCameraWithInput) {
	//Create ECS and components needed for the test.
	ecs::EntityComponentSystem m_ecs;
	ecs::components::MouseComponent mouse;
	ecs::components::KeyboardComponent keyboard;
	//Set a constant input on the mouse and keyboard to test against.
	mouse.diffFloat2.x = 50.0f;
	mouse.diffFloat2.y = 50.0f;
	keyboard.A = true;
	keyboard.W = true;
	//Create the entity holding the mouse and keyboard components and create the camera and its system.
	m_ecs.createEntity(mouse, keyboard);
	CameraFunctions::CreateDevCamera(m_ecs);
	m_ecs.createSystem<ecs::systems::UpdateCameraSystem>();
	//Update the system once.
	m_ecs.update(0.01f);
	//Fetch the updated components of the camera.
	TransformComponent* transform = (ecs::components::TransformComponent*)m_ecs.getAllComponentsOfType(ecs::components::TransformComponent::typeID).next();
	CameraComponent* camera = (ecs::components::CameraComponent*)m_ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID).next();

	//Test to see that the new values of the components are what we expect them to be with the given input.
	EXPECT_NEAR(transform->position.x, -0.09f, 0.2f);
	EXPECT_NEAR(transform->position.y, 9.9f, 0.2f);
	EXPECT_NEAR(transform->position.z, 0.2f, 0.2f);
	EXPECT_NEAR(transform->rotation.x, 0.5f, 0.2f);
	EXPECT_NEAR(transform->rotation.y, 0.5f, 0.2f);
	EXPECT_NEAR(transform->rotation.z, 0.0f, 0.2f);
	EXPECT_NEAR(transform->scale.x, 1.0f, 0.2f);
	EXPECT_NEAR(transform->scale.y, 1.0f, 0.2f);
	EXPECT_NEAR(transform->scale.z, 1.0f, 0.2f);

	EXPECT_NEAR(camera->target.x, 0.3f, 0.2f);
	EXPECT_NEAR(camera->target.y, 9.4f, 0.2f);
	EXPECT_NEAR(camera->target.z, 1.0f, 0.2f);
	EXPECT_NEAR(camera->up.x, 0.2f, 0.2f);
	EXPECT_NEAR(camera->up.y, 0.8f, 0.2f);
	EXPECT_NEAR(camera->up.z, 0.4f, 0.2f);
	EXPECT_NEAR(camera->forward.x, 0.4f, 0.2f);
	EXPECT_NEAR(camera->forward.y, -0.4f, 0.2f);
	EXPECT_NEAR(camera->forward.z, 0.7f, 0.2f);
	EXPECT_NEAR(camera->right.x, 0.8f, 0.2f);
	EXPECT_NEAR(camera->right.y, 0.0f, 0.2f);
	EXPECT_NEAR(camera->right.z, -0.4f, 0.2f);
}

// Test if an int is initialized properly and that set/get and ToString
// functionality works as expected.
TEST(TestDebugInfo, AddInt) {
	// Is Fps equal to default 0?
	EXPECT_EQ(DInfo::Graphics::Fps, 0);
	// Check if strings works
	EXPECT_STREQ(DInfo::Graphics::Fps.ToString().c_str(), "0");
	// Set Fps to 60
	DInfo::Graphics::Fps = 60;
	// Check if it changed
	EXPECT_EQ(DInfo::Graphics::Fps, 60);
	EXPECT_STREQ(DInfo::Graphics::Fps.ToString().c_str(), "60");
}

// Test if an string is initialized properly and that set/get and ToString
// functionality works as expected.
TEST(TestDebugInfo, AddString) {
	// Is Title an empty string?
	EXPECT_STREQ(DInfo::Graphics::Title.ToString().c_str(), "");
	std::string title = DInfo::Graphics::Title;
	EXPECT_STREQ(title.c_str(), "");
	// Set Title to "Couch Commanders"
	DInfo::Graphics::Title = "Couch Commanders";
	// Check if it got changed
	EXPECT_STREQ(DInfo::Graphics::Title.ToString().c_str(), "Couch Commanders");
	title = DInfo::Graphics::Title;
	EXPECT_STREQ(title.c_str(), "Couch Commanders");
}

// Test if the macro DINFO_SET only sets variables in debug configuration
TEST(TestDebugInfo, DInfo_SetOnlyInDebug) {
	// Set Title to "Oh boy, cheese!" using DINFO_SET
	DINFO_SET(DInfo::Graphics::Title, "Oh boy, cheese!");
#ifdef _DEBUG
	// This only runs if _DEBUG is defined. Check if the string is correct
	EXPECT_STREQ(DInfo::Graphics::Title.ToString().c_str(), "Oh boy, cheese!");
#else
	// This only runs if _DEBUG is not defined. Check if the string is incorrect
	EXPECT_STRNE(DInfo::Graphics::Title.ToString().c_str(), "Oh boy, cheese!");
#endif // _DEBUG

}