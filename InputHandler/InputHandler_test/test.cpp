#include "pch.h"
#include "..//InputHandler/includes/InputBackend.h"	
#include "InterpretWebSystems.h"

// Crashes when tests try to make multiple InputBackends so this is how you init the input Backend
InputBackend* gInputBackend;


int main(int argc, char** argv) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}



TEST(Input, InitInputAndWeb) 
{
	gInputBackend = new InputBackend();
}

// All updates can run without crash
TEST(Input, updates)
{
	(gInputBackend->updateKeyboard());
	(gInputBackend->updateMouse());
	(gInputBackend->updateWeb());
}

// All keys have been init
TEST(Input, keys)
{
	EXPECT_TRUE(gInputBackend->mpExitKey->key.index > -1);
	EXPECT_TRUE(gInputBackend->mpRessetKey->key.index > -1);
	EXPECT_TRUE(gInputBackend->mpMouseRKey->key.index > -1);
	EXPECT_TRUE(gInputBackend->mpMouseLKey->key.index > -1);

	EXPECT_TRUE(gInputBackend->mpWsad->keyU.index > -1);
	EXPECT_TRUE(gInputBackend->mpWsad->keyD.index > -1);
	EXPECT_TRUE(gInputBackend->mpWsad->keyL.index > -1);
	EXPECT_TRUE(gInputBackend->mpWsad->keyR.index > -1);
}

//Checks if any key was selected, should not have any button pressed
TEST(Input, web)
{
	gInputBackend->updateWeb();
	for (size_t i = 0; i < 4; i++)
	{
		EXPECT_TRUE(gInputBackend->mpUserButton[0]->mButton == -1);
	}
	
}

// Sees if the ECS parts can be created
TEST(ECS, InitCompEntSys) 
{
	ecs::EntityComponentSystem rECS;
	
	(initInputECS(rECS, gInputBackend));
	
	rECS.update(0.3f);

	
}

// checks if the enetity was created 
TEST(ECS, TestEntityCreation)
{
	ecs::EntityComponentSystem rECS;

	initInputECS(rECS, gInputBackend);

	rECS.update(0.3f);

	(rECS.getEntity(1));

}

// checks if all systems was created
TEST(ECS, TestSystemCreation)
{
	ecs::EntityComponentSystem rECS;

	initInputECS(rECS, gInputBackend);

	rECS.update(0.3f);

	EXPECT_EQ(rECS.getTotalSystemCount(), 4);
}


// checks if all components was created
TEST(ECS, TestComponentCreation)
{
	ecs::EntityComponentSystem rECS;

	initInputECS(rECS, gInputBackend);

	rECS.update(0.3f);

	EXPECT_EQ(rECS.getTotalComponentCount(), 6);
}

TEST(ECSvsInput, updateALLSystems)
{
	ecs::EntityComponentSystem rECS;

	initInputECS(rECS, gInputBackend);

	gInputBackend->mpWsad->keyU.pressed = true;
	gInputBackend->mpWsad->keyD.pressed = true;
	gInputBackend->mpRessetKey->key.pressed = true;

	rECS.update(0.3f);

	EXPECT_FALSE(gInputBackend->mpWsad->keyU.pressed);
	EXPECT_FALSE(gInputBackend->mpWsad->keyD.pressed);
	EXPECT_FALSE(gInputBackend->mpRessetKey->key.pressed);
}

TEST(ECSvsInput, updateKBWithInpBackend)
{
	ecs::EntityComponentSystem rECS;

	initInputECS(rECS, gInputBackend);

	gInputBackend->mpWsad->keyU.pressed = true;
	gInputBackend->mpWsad->keyD.pressed = true;
	gInputBackend->mpRessetKey->key.pressed = true;

	gInputBackend->updateKeyboard();

	EXPECT_FALSE(gInputBackend->mpWsad->keyU.pressed);
	EXPECT_FALSE(gInputBackend->mpWsad->keyD.pressed);
	EXPECT_FALSE(gInputBackend->mpRessetKey->key.pressed);
}

TEST(ECSvsInput, updateMouseSYS)
{
	ecs::EntityComponentSystem rECS;

	initInputECS(rECS, gInputBackend);

	gInputBackend->mpMouse->mDiffLength = 10000;

	rECS.update(0.3f);

	EXPECT_EQ(gInputBackend->mpMouse->mDiffLength, 0.0f);
}

TEST(ECSvsInput, updateMouseWithInpBackend)
{
	ecs::EntityComponentSystem rECS;

	initInputECS(rECS, gInputBackend);

	gInputBackend->mpMouse->mDiffLength = 10000;

	gInputBackend->updateMouse();

	EXPECT_EQ(gInputBackend->mpMouse->mDiffLength, 0.0f);
}





// Checks that the system was created correctly
TEST(Interperter, InitECS) {

	ecs::EntityComponentSystem mecs;

	initInputECS(mecs, gInputBackend);

	int pre = mecs.getTotalSystemCount();
	mecs.createSystem<ecs::systems::ChangeFSMSystem>();

	EXPECT_EQ(mecs.getTotalSystemCount(), (pre+1));

	mecs.update(0.1f);
}

// Checks that no fatal error occured when the event was created
TEST(Interperter, CreateECSEvent) {

	ecs::EntityComponentSystem mecs;

	ecs::systems::ChangeFSMSystem* psys = mecs.createSystem<ecs::systems::ChangeFSMSystem>();

	ecs::events::ChangeUserStateEvent testEvent;
	testEvent.newState = STATE::MOVE;
	testEvent.playerId = PLAYER::PLAYER3;

	EXPECT_NO_FATAL_FAILURE(mecs.createEvent(testEvent));
}










TEST(Memory, deleteInput)
{
	delete gInputBackend;
}




