#include "..//includes/rendering/RenderManager.h"

#ifdef _DEBUG
#pragma comment(lib, "RenderEngine_d.lib")
#else
#pragma comment(lib, "RenderEngine.lib")
#endif // DEBUG

int main()
{
	using namespace rendering;

	rendering::RenderManager mng;
	mng.Initialize();

	mng.Draw();

	mng.Destroy();

	system("pause");

	return 0;
}