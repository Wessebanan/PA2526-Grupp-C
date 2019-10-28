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
	rSystemPointers.UIpreSys	= rECS.createSystem<ecs::systems::UIPreRenderSystem>(8);
	rSystemPointers.UIBitmapSys = rECS.createSystem<ecs::systems::UIBitmapSystem>(9);
	rSystemPointers.UITextSys	= rECS.createSystem<ecs::systems::UITextSystem>(9);
	rSystemPointers.UIrectSys	= rECS.createSystem<ecs::systems::UIRectSystem>(9);
	rSystemPointers.UIDebugSys	= rECS.createSystem<ecs::systems::UIDebugSystem>(9);
	rSystemPointers.UIpostSys	= rECS.createSystem<ecs::systems::UIPostRenderSystem>(9);
	my_d2d = new Direct2D;

	ID3D11Texture2D* backbuffer;
	IDXGISwapChain1* swapchain;
	IDXGIDevice* dxgi_device;
	graphics::internal::D3D11_DEVICE_HANDLE handle;
	graphics::internal::GetD3D11(&handle); //Get handle from DX11
	handle.pDevice4->QueryInterface(&dxgi_device); //use the handles device to get a dxgi device
	my_d2d->InitDeviceAndContext(dxgi_device); //use to dxgi device to create Direct2D device and device context
	dxgi_device->Release();//release after use
	graphics::internal::GetSwapChain(&swapchain);//Get swapchain
	swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));//Use swapchain to get backbuffer as a dx11 texture
	swapchain->Release();
	BindTextureToBitmap(my_d2d, backbuffer);//Turn the texture to a surface then bind that surface to a Direct2D bitmap then draw things on that bitmap which is the backbuffer
	backbuffer->Release();
	//my_d2d->CreateHwndRenderTarget(window, graphics::GetClientResolution(window).x, graphics::GetClientResolution(window).y);
	rSystemPointers.UIpreSys->mpD2D			= 
		rSystemPointers.UITextSys->mpD2D	= 
		rSystemPointers.UIpostSys->mpD2D	= 
		rSystemPointers.UIrectSys->mpD2D	=
		rSystemPointers.UIBitmapSys->mpD2D  = 
		rSystemPointers.UIDebugSys->mpD2D	= my_d2d;

	ecs::components::UITextComponent text_comp;//temp debug text
	text_comp.mStrText = "adkguhadigsdgkasgd";

	rECS.createEntity(text_comp);

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
	ecs::components::UITextComponent command_text_comp;
	command_text_comp.mStrText = "NOT SET YET";
	ecs::components::UIDrawPosComponent text_pos_comp;
	ecs::components::UIDrawColorComponent text_color_comp;

	HWND hwnd = GetActiveWindow();
	LPRECT rect = nullptr;
	GetClientRect(hwnd, rect);

	FLOAT client_width = 1800;
	FLOAT client_height = 1000;

	if (rect != nullptr)
	{
		client_width = rect->right;
		client_height = rect->bottom;
	}

	FLOAT text_height = 100;
	FLOAT text_width = 200;
	for(int i = 0; i<4;i++)
	{
		switch (i)
		{
		case 0:
			text_pos_comp.mDrawArea.top = 0;
			text_pos_comp.mDrawArea.bottom = text_height;
			text_pos_comp.mDrawArea.left = 0;
			text_pos_comp.mDrawArea.right = text_width;

			text_color_comp.mColor = brushColors::Red;
			break;
		case 1:
			text_pos_comp.mDrawArea.top = 0;
			text_pos_comp.mDrawArea.bottom = text_height;
			text_pos_comp.mDrawArea.left = client_width - text_width;
			text_pos_comp.mDrawArea.right = client_width;

			text_color_comp.mColor = brushColors::Green;
			break;
		case 2:
			text_pos_comp.mDrawArea.top = client_height - text_height;
			text_pos_comp.mDrawArea.bottom = client_height;
			text_pos_comp.mDrawArea.left = 0;
			text_pos_comp.mDrawArea.right = text_width;

			text_color_comp.mColor = brushColors::Blue;
			break;
		case 3:
			text_pos_comp.mDrawArea.top = client_height - text_height;
			text_pos_comp.mDrawArea.bottom = client_height;
			text_pos_comp.mDrawArea.left = client_width - text_width;
			text_pos_comp.mDrawArea.right = client_width;

			text_color_comp.mColor = brushColors::Purple;
			break;
		default:
			text_pos_comp.mDrawArea.top = 100;
			text_pos_comp.mDrawArea.bottom = 200;
			text_pos_comp.mDrawArea.left = 100;
			text_pos_comp.mDrawArea.right = 200;

			text_color_comp.mColor = brushColors::Black;
			break;
		}
	}
	rECS.createEntity(command_text_comp, text_pos_comp, text_color_comp);
}
