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
#include "UISystems.h"

#include "initGameECS.h"


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
	// Memmory Management
	//initMem()
	
	ecs::EntityComponentSystem ecs;

	// Game systems for loops and states
	initGameECS(ecs);



	/// **** Bigger systems *****

	// Input
	InputBackend* inp = new InputBackend;
	initInputECS(ecs,inp);


	// Camera
	CameraFunctions::CreateDevCamera(ecs);
	//CameraFunctions::CreateDevCamera(ecs);


	// ModelLoader
	//initModelLoader(ecs)


	// Physics
	//initPhysics(ecs)
	

	// Sound
	//initSound(ecs)
	

	// GraphicsEngine
	//initGraphicsEngine(ecs)


	// UI
	Direct2D* d2d = new Direct2D();
	ecs::init::InitUISystems(ecs, &d2d);

	// Pathfinding
	//initPathfinding(ecs)


	// AI
	//initAI(ecs);



	/// **** Game objects *****

	// Grid
	//initGrid(ecs)

	// Armies
	//initArmies(ecs)

	// 

	bool loop = true;
	
	while (loop)
	{
		// Update ECS
		ecs.update(0.1f);


		// Gameloop outside of ecs




		// Check if game should stop
		if ( 666 == ((ecs::components::GameStateComponent*)ecs.getAllComponentsOfType(ecs::components::GameStateComponent::typeID).next())->gameState )
			loop = false;
	}


	delete inp;
	return 0;
}