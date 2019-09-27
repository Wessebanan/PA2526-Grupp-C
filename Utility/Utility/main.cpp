#include "InitInputHandler.h"
#include "CameraFunctions.h"
#include <iostream>
#include <chrono>
#include <thread>

int main()
{
	ecs::EntityComponentSystem mEcs;
	InputBackend ibe;
	CameraFunctions::CreateDevCamera(mEcs);
	mEcs.createSystem<ecs::systems::UpdateCameraSystem>();
	initInputECS(mEcs, &ibe);
	while (true)
	{
		mEcs.update(0.1f);
	}
	//CameraFunctions::CreateCameraSystems(mEcs);

	return 0;
}