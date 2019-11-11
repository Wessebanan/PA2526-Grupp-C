#pragma once
#include "ecs.h"
#include "Direct2D.h"
#include "UISystems.h"
#include "../../Graphics/includes/Window.h"
#include "..//../Graphics/includes/GraphicsInterface.h"

struct TempUISystemPtrs
{
	ecs::systems::UIPreRenderSystem* UIpreSys;
	ecs::systems::UIBitmapSystem* UIBitmapSys;
	ecs::systems::UISolidRectSystem* UISolid;
	ecs::systems::UITextSystem* UITextSys;
	ecs::systems::UIDebugSystem* UIDebugSys;
	ecs::systems::UIPostRenderSystem* UIpostSys;
	ecs::systems::UIRectSystem* UIrectSys;
};

void BindTextureToBitmap(Direct2D* d2d,ID3D11Texture2D* texture);
void initArmyText(ecs::EntityComponentSystem& rECS);


void InitUI(ecs::EntityComponentSystem& rECS, TempUISystemPtrs& rSystemPointers)
{
	Direct2D* my_d2d;
	rSystemPointers.UIpreSys = rECS.createSystem<ecs::systems::UIPreRenderSystem>(8);
	rSystemPointers.UISolid = rECS.createSystem<ecs::systems::UISolidRectSystem>(9);
	rSystemPointers.UITextSys = rECS.createSystem<ecs::systems::UITextSystem>(9);
	rSystemPointers.UIrectSys = rECS.createSystem<ecs::systems::UIRectSystem>(9);
	rSystemPointers.UIDebugSys = rECS.createSystem<ecs::systems::UIDebugSystem>(9);
	rSystemPointers.UIBitmapSys = rECS.createSystem<ecs::systems::UIBitmapSystem>(9);
	rSystemPointers.UIpostSys = rECS.createSystem<ecs::systems::UIPostRenderSystem>(9);
	my_d2d = new Direct2D;

	ID3D11Texture2D* p_backbuffer;
	IDXGISwapChain1* p_swapchain;
	IDXGIDevice* p_dxgi_device;
	graphics::internal::D3D11_DEVICE_HANDLE handle;
	graphics::internal::GetD3D11(&handle); //Get handle from DX11
	handle.pDevice4->QueryInterface(&p_dxgi_device); //use the handles device to get a dxgi device
	my_d2d->InitDeviceAndContext(p_dxgi_device); //use to dxgi device to create Direct2D device and device context
	p_dxgi_device->Release();//release after use
	graphics::internal::GetSwapChain(&p_swapchain);//Get swapchain
	p_swapchain->GetBuffer(0, IID_PPV_ARGS(&p_backbuffer));//Use swapchain to get backbuffer as a dx11 texture
	p_swapchain->Release();
	BindTextureToBitmap(my_d2d, p_backbuffer);//Turn the texture to a surface then bind that surface to a Direct2D bitmap then draw things on that bitmap which is the backbuffer
	p_backbuffer->Release();
	//my_d2d->CreateHwndRenderTarget(window, graphics::GetClientResolution(window).x, graphics::GetClientResolution(window).y);
	rSystemPointers.UIpreSys->mpD2D			=
		rSystemPointers.UISolid->mpD2D		=
		rSystemPointers.UITextSys->mpD2D	=
		rSystemPointers.UIpostSys->mpD2D	=
		rSystemPointers.UIrectSys->mpD2D	=
		rSystemPointers.UIDebugSys->mpD2D	= 
		rSystemPointers.UIBitmapSys->mpD2D	= my_d2d;

	//initArmyText(rECS); //maybe use later donno 
}
void BindTextureToBitmap(Direct2D* d2d, ID3D11Texture2D* texture)
{
	IDXGISurface* surface;
	ID2D1Bitmap1* bitmap;

	texture->QueryInterface(&surface);


	d2d->GetpContext()->CreateBitmapFromDxgiSurface(
		surface,
		NULL,
		&bitmap
	);
	d2d->setBackbufferBitmap(bitmap);
	bitmap->Release();
	surface->Release();
}

void initArmyText(ecs::EntityComponentSystem& rECS)
{
	rECS.createSystem<ecs::systems::UIUpdateSystem>();
}
