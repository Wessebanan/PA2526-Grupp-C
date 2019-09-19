#include "CameraFunctions.h"
#include "InitInputHandler.h"

int main()
{
	InputBackend input;
	ecs::EntityComponentSystem mEcs;
	CameraFunctions::CreateDevCamera(mEcs);
	CameraFunctions::CreateCameraSystems(mEcs);

	return 0;
}