#include "ecs.h"
#include "InitInput.h"
#include "InitUI.h"
#include "Direct2D.h"

using namespace ecs;

int main()
{
	Direct2D* my_d2d;
	EntityComponentSystem ecs;
	TempUISystemPtrs my_UI_systems;

	initInput(ecs);
	InitUI(ecs, &my_d2d, my_UI_systems);



	ecs.update(0.0f);

	return 0;
}