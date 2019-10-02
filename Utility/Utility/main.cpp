#include "CameraFunctions.h"

int main()
{
	ecs::EntityComponentSystem mEcs;
	CameraFunctions::CreateDevCamera(mEcs);
	CameraFunctions::CreateCameraSystems(mEcs);

	return 0;
}