#include <ecs.h>
#include "InitInputHandler.h"
#include "Mesh.h"
#include <DeviceInterface.h>
#include <UtilityFunctions.h>
#include <CameraFunctions.h>
#include <GridFunctions.h>
#include <DebugInfo.h>
#include "Shaders.h"
#include "Fbx_Loader.h"


#define GRID_WIDTH 12
#define GRID_HEIGHT 12
#define GRID_HEX_SIZE 8

namespace DInfo
{
	namespace Graphics
	{
		DINFO_ADD_PARAM(FPS, int, 0);
	}
}

//#pragma comment (lib, "GraphicsEngine_d")
using namespace graphics;


int main()
{
	ecs::EntityComponentSystem ecs;

	// Input
	InputBackend* inp = new InputBackend;
	initInputECS(ecs,inp);


	// Camera
	CameraFunctions::CreateDevCamera(ecs);
	//CameraFunctions::CreateCameraSystems(ecs);


	// ModelLoader
	//initModelLoader()


	// Physics
	//initPhysics
	

	// Grid
	//initGrid(ecs)
	

	// GraphicsEngine
	//initGraphicsEngine(ecs)



	
	while (true)
	{
		ecs.update(0.1f);
	}
	
	delete inp;
	return 0;
}