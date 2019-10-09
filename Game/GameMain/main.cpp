#include "ecs.h"
#include "InitInput.h"
#include "InitSound.h"

using namespace ecs;

int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	EntityComponentSystem ecs;

	initInput(ecs);

	InitSound(ecs);

	ecs.update(0.0f);

	return 0;
}