#include "ecs.h"
#include "Input/InitInput.h"
#include "Rendering/InitMeshes.h"

using namespace ecs;

int main()
{
	EntityComponentSystem ecs;

	initInput(ecs);

	// Init Render Manger outside to use it elsewhere
	RenderManager mng;

	InitMeshes(ecs, mng);



	graphics::PresentWindow* pWnd = mng.GetPresentWindow();

	pWnd->Show();
	while (pWnd->IsOpen())
	{
		if (!pWnd->Update())
		{
			mng.Clear(0.2f, 0.1f, 0.1f);



			mng.Draw();
			pWnd->Present();
		}
	}
	ecs.update(0.0f);

	return 0;
}