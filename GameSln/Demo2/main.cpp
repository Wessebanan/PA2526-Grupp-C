#include <ecs.h>
#include "InitInputHandler.h"
#include <Mesh.h>
#include <DeviceInterface.h>
#include <UtilityFunctions.h>
#include <CameraFunctions.h>
#include <GridFunctions.h>
#include <DebugInfo.h>
#include "Shaders.h"


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
	ecs::CompTypeMemDesc ecsMemDesc[] = {
		{ TileComponent::typeID, TileComponent::size, GRID_WIDTH * GRID_HEIGHT },
		{ TransformComponent::typeID, TransformComponent::size, GRID_WIDTH * GRID_HEIGHT },
	};

	ecs::ECSDesc ecsDesc;
	ecsDesc.compTypeCount = 2;
	ecsDesc.compTypeMemDescs = ecsMemDesc;
	ecsDesc.systemLayerCount = 10;

	ecs::EntityComponentSystem ecs;
	ecs.initialize(ecsDesc);

	// Input
	InputBackend* inp = new InputBackend;
	initInputECS(ecs,inp);

	// Camera
	CameraFunctions::CreateCameraSystems(ecs);
	CameraFunctions::CreateDevCamera(ecs);


	// Rendereing
	DeviceInterface* pDevice;
	PresentWindow* pWindow;
	GraphicsPipeline* pPipeline, * pPipeline2;
	CreateDeviceInterface(&pDevice);
	BufferRegion hexMeshBuffReg, dudeMeshBuffReg;

	CreateDeviceInterface(&pDevice);
	RenderContext* pContext = pDevice->GetRenderContext();
	RenderTarget backBuffer;
	DepthBuffer depthBuffer;
	pDevice->CreateDepthBuffer(1280, 720, &depthBuffer);

	pDevice->CreatePresentWindow(1280, 720, "D3D11", &backBuffer, &pWindow);

	pDevice->CreatePipeline(gVertexShader, gPixelShader, &pPipeline);
	pDevice->CreatePipeline(gVertexShader2, gPixelShader2, &pPipeline2);

	pContext->SetViewport(0, 0, 1280, 720);
	pContext->SetGraphicsPipeline(pPipeline);


	ModelLoader::Mesh hexMesh;

	return 0;
}