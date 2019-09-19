#include "pch.h"
#include "..//InputHandler/includes/InitInputHandler.h"

// Crashes when tests try to make multiple InputBackends so this is how you init the input Backend
InputBackend* gInputBackend;
TEST(Input, InitInputAndWeb) 
{
	EXPECT_NO_FATAL_FAILURE(gInputBackend = new InputBackend());
}

// All updates can run without crash
TEST(Input, updates)
{
	EXPECT_NO_FATAL_FAILURE(gInputBackend->updateKeyboard());
	EXPECT_NO_FATAL_FAILURE(gInputBackend->updateMouse());
	EXPECT_NO_FATAL_FAILURE(gInputBackend->updateWeb());
}

// All keys have been init
TEST(Input, keys)
{
	EXPECT_TRUE(gInputBackend->exitKey->key.index > -1);
	EXPECT_TRUE(gInputBackend->ressetKey->key.index > -1);
	EXPECT_TRUE(gInputBackend->mouseRKey->key.index > -1);
	EXPECT_TRUE(gInputBackend->mouseLKey->key.index > -1);

	EXPECT_TRUE(gInputBackend->wsad->keyU.index > -1);
	EXPECT_TRUE(gInputBackend->wsad->keyD.index > -1);
	EXPECT_TRUE(gInputBackend->wsad->keyL.index > -1);
	EXPECT_TRUE(gInputBackend->wsad->keyR.index > -1);
}

//Checks if any key was selected, should not have any button pressed
TEST(Input, web)
{
	gInputBackend->updateWeb();
	for (size_t i = 0; i < 4; i++)
	{
		EXPECT_FALSE(gInputBackend->playerControll[i]->keyU.pressed);
	}
	
}

// Sees if the ECS parts can be created
TEST(ECS, InitCompEntSys) 
{
	ecs::EntityComponentSystem rECS;
	
	EXPECT_NO_FATAL_FAILURE(initInputECS(rECS, gInputBackend));
	
	rECS.update(0.3f);

	
}

// checks if the enetity was created 
TEST(ECS, TestEntityCreation)
{
	ecs::EntityComponentSystem rECS;

	initInputECS(rECS, gInputBackend);

	rECS.update(0.3f);

	EXPECT_NO_FATAL_FAILURE(rECS.getEntity(1));

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

	EXPECT_EQ(rECS.getTotalComponentCount(), 5);
}