#include "InitInputHandler.h"
#include "ecs.h"
#include "includes/InterpretWebSystems.h"

int main()
{
	ecs::EntityComponentSystem mECS;

	cout << "yoloSwag" << endl;

	mECS.createSystem<ecs::systems::ChangeFSMSystem>();

	return 0;
}