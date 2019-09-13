#include "CameraFunctions.h"

int main()
{
	ecs::EntityComponentSystem mEcs;
	CameraFunctions::InitDevCamera(mEcs);
	CameraFunctions::InitCameraSystems(mEcs);

	return 0;
}