#include "InitInputHandler.h"
#include "CameraFunctions.h"
#include <iostream>
#include <chrono>
#include <thread>

int main()
{
	ecs::EntityComponentSystem mEcs;
	CameraFunctions::CreateDevCamera(mEcs);
	mEcs.createSystem<ecs::systems::UpdateCameraSystem>();
	initInputECS(mEcs);
	while (true)
	{
		mEcs.update(0.1f);
	}
	//CameraFunctions::CreateCameraSystems(mEcs);

	return 0;
}