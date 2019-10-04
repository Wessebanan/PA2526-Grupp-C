#include "ecs.h"
#include "InitInput.h"

using namespace ecs;

int main()
{
	EntityComponentSystem ecs;

	initInput(ecs);



	ecs.update(0.0f);

	return 0;
}