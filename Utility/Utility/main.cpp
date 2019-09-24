#include "CameraFunctions.h"
#include "InitInputHandler.h"
#include <iostream>

int main()
{
	ecs::EntityComponentSystem mEcs;
	CameraFunctions::CreateDevCamera(mEcs);
	CameraFunctions::CreateCameraSystems(mEcs);
	InputBackend ibe;
	initInputECS(mEcs, &ibe);

	int dank = mEcs.getComponentCountOfType(ecs::components::MouseComponent::typeID);

	while (true)
	{
		ecs::ComponentIterator it = mEcs.getAllComponentsOfType(ecs::components::MouseComponent::typeID);

		ecs::components::MouseComponent *p_mouse = (ecs::components::MouseComponent*)it.next();

		std::cout << "------------- MACKES PRINT -------------" << endl;
		std::cout << " Mouse x: " << p_mouse->diffx << endl;
		std::cout << " Mouse y: " << p_mouse->diffy << endl;
		std::cout << "----------------------------------------" << endl;

		mEcs.update(0.1f);
		//std::cout << "x: " << ibe.mouse->diffFloat2.x << std::endl;
		//std::cout << "y: " << ibe.mouse->diffFloat2.y << std::endl;
	}

	return 0;
}