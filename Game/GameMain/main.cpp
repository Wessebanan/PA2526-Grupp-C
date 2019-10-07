#include "ecs.h"
#include "Input/InitInput.h"

using namespace ecs;

int main()
{
	EntityComponentSystem ecs;

	initInput(ecs);



	ecs.update(0.0f);

	return 0;
}