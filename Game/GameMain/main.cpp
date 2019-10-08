#include "ecs.h"
#include "InitInput.h"
#include "InitSound.h"

using namespace ecs;

int main()
{
	EntityComponentSystem ecs;

	initInput(ecs);

	InitSound(ecs);

	ecs.update(0.0f);

	return 0;
}