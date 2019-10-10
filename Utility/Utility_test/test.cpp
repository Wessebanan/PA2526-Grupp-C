#include "gtest/gtest.h"
#include "ecs.h"
#include "../../Game/GameMain/gameUtility/UtilityEcsFunctions.h"
#include "../../Game/GameMain/gameUtility/CameraEcsFunctions.h"
#include "../../Game/GameMain/gameUtility/CameraSystems.h"
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
	DirectX::XMMATRIX world = UtilityEcsFunctions::GetWorldMatrix(tc);
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
	CameraEcsFunctions::CreateDevCamera(mEcs);

	size_t numberOfTc = mEcs.getComponentCountOfType(ecs::components::TransformComponent::typeID);
	size_t numberOfCc = mEcs.getComponentCountOfType(ecs::components::CameraComponent::typeID);

	EXPECT_EQ(numberOfCc + numberOfTc, 2);
	EXPECT_EQ(numberOfCc, 1);
	EXPECT_EQ(numberOfTc, 1);
}

TEST(CameraFunctions, MoveCameraWithInput) {
	//Create ECS and components needed for the test.
	ecs::EntityComponentSystem temp_ecs;
	ecs::components::MouseComponent mouse;
	ecs::components::KeyboardComponent keyboard;
	//Set a constant input on the mouse and keyboard to test against.
	mouse.diffFloat2.x = 50.0f;
	mouse.diffFloat2.y = 50.0f;
	keyboard.A = true;
	keyboard.W = true;
	//Create the entity holding the mouse and keyboard components and create the camera and its system.
	temp_ecs.createEntity(mouse, keyboard);
	CameraEcsFunctions::CreateDevCamera(temp_ecs);
	temp_ecs.createSystem<ecs::systems::UpdateCameraSystem>();
	//Update the system once.
	temp_ecs.update(0.01f);
	//Fetch the updated components of the camera.
	TransformComponent* transform = (ecs::components::TransformComponent*)temp_ecs.getAllComponentsOfType(ecs::components::TransformComponent::typeID).next();
	CameraComponent* camera = (ecs::components::CameraComponent*)temp_ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID).next();

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

TEST(CameraFunctions, ResetCamera) {
	//Create ECS and components needed for the test.
	ecs::EntityComponentSystem temp_ecs;
	ecs::components::MouseComponent mouse;
	ecs::components::KeyboardComponent keyboard;
	//Set a constant input on the mouse and keyboard to test against.
	mouse.diffFloat2.x = 50.0f;
	mouse.diffFloat2.y = 50.0f;
	keyboard.A = true;
	keyboard.W = true;
	//Create the entity holding the mouse and keyboard components and create the camera and its system.
	temp_ecs.createEntity(mouse, keyboard);
	CameraEcsFunctions::CreateDevCamera(temp_ecs);
	temp_ecs.createSystem<ecs::systems::UpdateCameraSystem>();
	//Update the system once to move the camera.
	temp_ecs.update(0.01f);
	//Fetch the updated components of the camera and the keyboard Component.
	KeyboardComponent* p_keyboard = (ecs::components::KeyboardComponent*)temp_ecs.getAllComponentsOfType(ecs::components::KeyboardComponent::typeID).next();
	TransformComponent* p_transform = (ecs::components::TransformComponent*)temp_ecs.getAllComponentsOfType(ecs::components::TransformComponent::typeID).next();
	CameraComponent* p_camera = (ecs::components::CameraComponent*)temp_ecs.getAllComponentsOfType(ecs::components::CameraComponent::typeID).next();
	//Set reset button to true and update the system again.
	p_keyboard->R = true;
	temp_ecs.update(0.01f);

	//Test to see that the new values of the components are what we expect them to be after the reset.
	EXPECT_NEAR(p_transform->position.x, CameraDefines::originalPosition.x, 0.1f);
	EXPECT_NEAR(p_transform->position.y, CameraDefines::originalPosition.y, 0.1f);
	EXPECT_NEAR(p_transform->position.z, CameraDefines::originalPosition.z, 0.1f);
	EXPECT_NEAR(p_transform->rotation.x, CameraDefines::originalRotation.x, 0.1f);
	EXPECT_NEAR(p_transform->rotation.y, CameraDefines::originalRotation.y, 0.1f);
	EXPECT_NEAR(p_transform->rotation.z, CameraDefines::originalRotation.z, 0.1f);
	EXPECT_NEAR(p_transform->scale.x, CameraDefines::originalScale.x, 0.1f);
	EXPECT_NEAR(p_transform->scale.y, CameraDefines::originalScale.y, 0.1f);
	EXPECT_NEAR(p_transform->scale.z, CameraDefines::originalScale.z, 0.1f);

	EXPECT_NEAR(p_camera->target.x, CameraDefines::originalTarget.x, 0.1f);
	EXPECT_NEAR(p_camera->target.y, CameraDefines::originalTarget.y, 0.1f);
	EXPECT_NEAR(p_camera->target.z, CameraDefines::originalTarget.z, 0.1f);
	EXPECT_NEAR(p_camera->forward.x, CameraDefines::originalForward.x, 0.1f);
	EXPECT_NEAR(p_camera->forward.y, CameraDefines::originalForward.y, 0.1f);
	EXPECT_NEAR(p_camera->forward.z, CameraDefines::originalForward.z, 0.1f);
	EXPECT_NEAR(p_camera->up.x, CameraDefines::originalUp.x, 0.1f);
	EXPECT_NEAR(p_camera->up.y, CameraDefines::originalUp.y, 0.1f);
	EXPECT_NEAR(p_camera->up.z, CameraDefines::originalUp.z, 0.1f);
	EXPECT_NEAR(p_camera->right.x, CameraDefines::originalRight.x, 0.1f);
	EXPECT_NEAR(p_camera->right.y, CameraDefines::originalRight.y, 0.1f);
	EXPECT_NEAR(p_camera->right.z, CameraDefines::originalRight.z, 0.1f);
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