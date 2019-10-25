#pragma once
#include "ecs.h"
#include "Direct2D.h"
#include "UISystems.h"
#include "../../Graphics/includes/Window.h"

struct TempUISystemPtrs
{
	ecs::systems::UIPreRenderSystem* UIpreSys;
	ecs::systems::UIBitmapSystem* UIBitmapSys;
	ecs::systems::UITextSystem* UITextSys;
	ecs::systems::UIDebugSystem* UIDebugSys;
	ecs::systems::UIPostRenderSystem* UIpostSys;
	ecs::systems::UIRectSystem* UIrectSys;
};

void BindTextureToBitmap(Direct2D* d2d, ID3D11Texture2D* texture);


void InitUI(ecs::EntityComponentSystem& rECS, TempUISystemPtrs& rSystemPointers, HWND window)
{
	Direct2D* my_d2d;
	rSystemPointers.UIpreSys	= rECS.createSystem<ecs::systems::UIPreRenderSystem>(8);
	rSystemPointers.UIBitmapSys = rECS.createSystem<ecs::systems::UIBitmapSystem>(9);
	rSystemPointers.UITextSys	= rECS.createSystem<ecs::systems::UITextSystem>(9);
	rSystemPointers.UIrectSys	= rECS.createSystem<ecs::systems::UIRectSystem>(9);
	rSystemPointers.UIDebugSys	= rECS.createSystem<ecs::systems::UIDebugSystem>(9);
	rSystemPointers.UIpostSys	= rECS.createSystem<ecs::systems::UIPostRenderSystem>(9);
	my_d2d = new Direct2D;
	IDXGIDevice* dxgi_device;
	
	graphics::internal::D3D11_DEVICE_HANDLE handle;
	graphics::internal::GetD3D11(&handle);
	graphics::internal::GetBackBuffer();
	
	//my_d2d->InitDeviceAndContext();
	//BindTextureToBitmap();
	//my_d2d->CreateHwndRenderTarget(window, graphics::GetClientResolution(window).x, graphics::GetClientResolution(window).y);
	rSystemPointers.UIpreSys->mpD2D			= 
		rSystemPointers.UITextSys->mpD2D	= 
		rSystemPointers.UIpostSys->mpD2D	= 
		rSystemPointers.UIrectSys->mpD2D	=
		rSystemPointers.UIBitmapSys->mpD2D  = 
		rSystemPointers.UIDebugSys->mpD2D	= my_d2d;

	ecs::components::UITextComponent text_comp;
	text_comp.mStrText = "adkguhadigsdgkasgd";
	//ecs::components::UIDrawPosComponent pos_comp;
	//pos_comp.mDrawArea.top = 100;
	//pos_comp.mDrawArea.bottom = 200;
	//pos_comp.mDrawArea.left = 100;
	//pos_comp.mDrawArea.right= 200;
	//ecs::components::UIDrawColorComponent color_comp;
	//color_comp.mColor = brushColors::Red;


	//rECS.createEntity(text_comp, pos_comp, color_comp);
	rECS.createEntity(text_comp);

	//ecs::components::UIThicknessComponent thick_comp;
	//thick_comp.mThickness = 1;
	//
	//rECS.createEntity(thick_comp, pos_comp, color_comp);

}
void BindTextureToBitmap(Direct2D* d2d, ID3D11Texture2D* texture)
{
	IDXGISurface* surface;
	ID2D1Bitmap1* bitmap;

	texture->QueryInterface(&surface);

	d2d->GetpContext()->CreateBitmapFromDxgiSurface(
		surface,
		nullptr,
		&bitmap
	);

	d2d->GetpContext()->SetTarget(bitmap);
	bitmap->Release();
	surface->Release();
}