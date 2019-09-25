#include "InitInputHandler.h"
#include "CameraFunctions.h"
#include <iostream>

//#include "InitInputHandler.h"
//#include "ecs.h"
//#include "DeviceInterface.h"
//#include "CameraFunctions.h"
//#include "ecsUser.h"

#include <chrono>
#include <thread>

int main()
{
	ecs::EntityComponentSystem mEcs;
	InputBackend ibe;
	initInputECS(mEcs, &ibe);
	CameraFunctions::CreateDevCamera(mEcs);
	mEcs.createSystem<ecs::systems::Mackes>();
	//CameraFunctions::CreateCameraSystems(mEcs);

	int dank = mEcs.getComponentCountOfType(ecs::components::MouseComponent::typeID);

	while (true)
	{
		/*ecs::ComponentIterator it = mEcs.getAllComponentsOfType(ecs::components::MouseComponent::typeID);

		ecs::components::MouseComponent *p_mouse = (ecs::components::MouseComponent*)it.next();

		std::cout << "------------- MACKES PRINT -------------" << endl;
		std::cout << " Mouse x: " << p_mouse->diffx << endl;
		std::cout << " Mouse y: " << p_mouse->diffy << endl;
		std::cout << "----------------------------------------" << endl;*/

		mEcs.update(0.1f);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		//std::cout << "x: " << ibe.mouse->diffFloat2.x << std::endl;
		//std::cout << "y: " << ibe.mouse->diffFloat2.y << std::endl;
	}

	return 0;
}