#pragma once
#include "ecs.h"
#include "Direct2D.h"
#include "UISystems.h"
#include "UIEvents.h"
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
	ecs::systems::UICountDownSystem* UICountDown;
};

void BindTextureToBitmap(Direct2D* d2d, ID3D11Texture2D* texture);
void InitGameOverlay(ecs::EntityComponentSystem& rECS, Direct2D* d2d);


void InitUI(ecs::EntityComponentSystem& rECS)
{
	Direct2D* my_d2d = new Direct2D;

	rECS.createSystem<ecs::systems::UnitComponentCreator>(0);
	rECS.createSystem<ecs::systems::UnitComponentPrinter>(0);
	rECS.createSystem<ecs::systems::LayerPrinter>(0);

	rECS.createSystem<ecs::systems::UIPreRenderSystem>(8)->mpD2D = my_d2d;
	rECS.createSystem<ecs::systems::UISolidRectSystem>(9)->mpD2D = my_d2d;
	rECS.createSystem<ecs::systems::UIBitmapSystem>(9)->mpD2D = my_d2d;
	rECS.createSystem<ecs::systems::UITextSystem>(9)->mpD2D = my_d2d;
	rECS.createSystem<ecs::systems::UIRectSystem>(9)->mpD2D = my_d2d;
	rECS.createSystem<ecs::systems::UIDebugSystem>(9)->mpD2D = my_d2d;
	rECS.createSystem<ecs::systems::UICountDownSystem>(9)->mpD2D = my_d2d;
	rECS.createSystem<ecs::systems::UIPostRenderSystem>(9)->mpD2D = my_d2d;
	rECS.createSystem<ecs::systems::UIEndOfRoundSystem>(9)->mpD2D = my_d2d;
	rECS.createSystem<ecs::systems::UIGameRestartSystem>(9)->mpD2D = my_d2d;

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

	ecs::components::UIBitmapComponent bitmap_comp;
	ecs::components::UIDrawPosComponent bitmap_pos_comp;

	// save this to switch between different opacities
	//bitmap_comp.mpBitmap = my_d2d->LoadImageToBitmap("../../UI/Resource/areaGrid75.png", "areaOverlay");
	//bitmap_comp.mpBitmap = my_d2d->LoadImageToBitmap("../../UI/Resource/areaGrid50.png", "areaOverlay");
	//bitmap_comp.mpBitmap = my_d2d->LoadImageToBitmap("../../UI/Resource/areaGrid30.png", "areaOverlay");
	bitmap_comp.mpBitmap = my_d2d->LoadImageToBitmap("../../UI/Resource/newArea50.png", "areaOverlay");

	bitmap_comp.mName = "areaOverlay";
	bitmap_pos_comp.mDrawArea.top = 150;
	bitmap_pos_comp.mDrawArea.left = 690;
	bitmap_pos_comp.mDrawArea.right = 1230;

	// Save these if you want to srtart with the overlay open or not
	bitmap_pos_comp.mDrawArea.bottom = 150;
	//bitmap_pos_comp.mDrawArea.bottom = 800;

	rECS.createEntity(bitmap_comp, bitmap_pos_comp);

	InitGameOverlay(rECS, my_d2d);


	// GUIDE
	bitmap_pos_comp.mDrawArea.top = 200;
	bitmap_pos_comp.mDrawArea.left = 300;
	bitmap_pos_comp.mDrawArea.right = 1600;

	// Save these if you want to srtart with the overlay open or not
	bitmap_pos_comp.mDrawArea.bottom = 200;
	//bitmap_pos_comp.mDrawArea.bottom = 1000;
	ID prev = -1;

	for (size_t i = 0; i < 8; i++)
	{
		//int i = 1;
		string name = "guide";
		name += to_string(i);
		string file_name = "../../UI/Resource/GAMEGUIDE (" + to_string(i) + ").png";

		// save this to switch between different opacities
		bitmap_comp.mpBitmap = my_d2d->LoadImageToBitmap(file_name, name);

		bitmap_comp.mName = name;

		GuideLinkerComponent link;
		link.mPrev = prev;
	
		prev = rECS.createEntity(bitmap_comp,bitmap_pos_comp, link)->getID();
	}

	ecs::components::UIDrawColorComponent text_color;
	ecs::components::UITextComponent text;
	ecs::components::UIDrawPosComponent text_pos;
	text_color.mColor = brushColors::White;
	text.text_size = 3;
	text.tag = DEBUGUI;
	text_pos.mDrawArea.left = graphics::GetDisplayResolution().x / 2 - 150;
	text_pos.mDrawArea.top = 10;
	text_pos.mDrawArea.right = graphics::GetDisplayResolution().x / 2 + 150;
	text_pos.mDrawArea.bottom = 210;
	rECS.createEntity(text, text_pos, text_color);

	{
		const int startX = 10.f;
		const int startY = 400.f;
		const int width = 200.f;

		UIECSLayerText layer;

		text.text_size = 0;
		text.tag = UITAG::NOTAG;
		text_color.mColor = White;

		text_pos.mDrawArea.top = 400.f;
		text_pos.mDrawArea.bottom = 800.f;

		for (int i = 0; i < 10; i++)
		{
			text.mStrText = L"INIT TEXT";
			layer.layer = i;
			text_pos.mDrawArea.left = startX + i * width;
			text_pos.mDrawArea.right = text_pos.mDrawArea.left + width;

			rECS.createEntity(text, text_color, text_pos, layer);
		}
	}
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
	d2d->SetBackbufferBitmap(bitmap);
	bitmap->Release();
	surface->Release();
}

void InitGameOverlay(ecs::EntityComponentSystem& rECS, Direct2D* d2d)
{
	ecs::components::UIBitmapComponent bitmap_comp;
	ecs::components::UIDrawPosComponent pos_comp;
	ecs::components::UIUnitReader unit_reader_comp;
	ecs::components::UIArmyReader army_reader_comp;

	// If more flexibilty of some bitmaps is wanted give the bitmap an UIArmyReader
	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/Background.png", "Background");
	bitmap_comp.mpTintedBitmap = nullptr;
	int width = d2d->GetBackbufferBitmap()->GetSize().width;
	int height = d2d->GetBackbufferBitmap()->GetSize().height;
	pos_comp.mDrawArea.left = 0;
	pos_comp.mDrawArea.top = 0;
	pos_comp.mDrawArea.right = 300;
	pos_comp.mDrawArea.bottom = 300;
	//d2d->SetBitmapTint(bitmap_comp.mpBitmap, NULL, 117, 1, 1);
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, NULL, 157, 2, 2);
	rECS.createEntity(bitmap_comp, pos_comp);//top left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/Background.png", "Background");
	bitmap_comp.mpTintedBitmap = nullptr;
	//d2d->SetBitmapTint(bitmap_comp.mpBitmap, NULL, 74, 1, 117);
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, NULL, 117, 4, 177);
	pos_comp.mDrawArea.left = width - 300;
	pos_comp.mDrawArea.top = 0;
	pos_comp.mDrawArea.right = width;
	pos_comp.mDrawArea.bottom = 300;
	rECS.createEntity(bitmap_comp, pos_comp); //top right

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/Background.png", "Background");
	bitmap_comp.mpTintedBitmap = nullptr;
	//d2d->SetBitmapTint(bitmap_comp.mpBitmap, NULL, 47, 62, 236);
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, NULL, 60, 72, 260);
	pos_comp.mDrawArea.left = 0;
	pos_comp.mDrawArea.top = height - 300;
	pos_comp.mDrawArea.right = 300;
	pos_comp.mDrawArea.bottom = height;
	rECS.createEntity(bitmap_comp, pos_comp);//bottom left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/Background.png", "Background");
	bitmap_comp.mpTintedBitmap = nullptr;
	//d2d->SetBitmapTint(bitmap_comp.mpBitmap, NULL, 0, 93, 5);
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, NULL, 0, 153, 10);
	pos_comp.mDrawArea.left = width - 300;
	pos_comp.mDrawArea.top = height - 300;
	pos_comp.mDrawArea.right = width;
	pos_comp.mDrawArea.bottom = height;
	rECS.createEntity(bitmap_comp, pos_comp);//bottom right

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/LeftMan.png", "LeftMan");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	bitmap_comp.to_draw = false;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 0, 0);
	pos_comp.mDrawArea.left = 0;
	pos_comp.mDrawArea.top = 0;
	pos_comp.mDrawArea.right = 300;
	pos_comp.mDrawArea.bottom = 300;
	unit_reader_comp.playerID = 0;
	unit_reader_comp.unitPlacement = UIUnitReader::UI_UNIT_LEFT;
	rECS.createEntity(bitmap_comp, pos_comp, unit_reader_comp);//top left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/LeftMan.png", "LeftMan");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 0, 255, 0);
	pos_comp.mDrawArea.left = width - 300;
	pos_comp.mDrawArea.top = 0;
	pos_comp.mDrawArea.right = width;
	pos_comp.mDrawArea.bottom = 300;
	unit_reader_comp.playerID = 1;
	unit_reader_comp.unitPlacement = UIUnitReader::UI_UNIT_LEFT;
	rECS.createEntity(bitmap_comp, pos_comp, unit_reader_comp); //top right

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/LeftMan.png", "LeftMan");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 0, 255, 0);
	pos_comp.mDrawArea.left = 0;
	pos_comp.mDrawArea.top = height - 300;
	pos_comp.mDrawArea.right = 300;
	pos_comp.mDrawArea.bottom = height;
	unit_reader_comp.playerID = 2;
	unit_reader_comp.unitPlacement = UIUnitReader::UI_UNIT_LEFT;
	rECS.createEntity(bitmap_comp, pos_comp, unit_reader_comp);//bottom left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/LeftMan.png", "LeftMan");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 0, 255, 0);
	pos_comp.mDrawArea.left = width - 300;
	pos_comp.mDrawArea.top = height - 300;
	pos_comp.mDrawArea.right = width;
	pos_comp.mDrawArea.bottom = height;
	unit_reader_comp.playerID = 3;
	unit_reader_comp.unitPlacement = UIUnitReader::UI_UNIT_LEFT;
	rECS.createEntity(bitmap_comp, pos_comp, unit_reader_comp);//bottom right

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/RightMan.png", "RightMan");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 0, 0);
	pos_comp.mDrawArea.left = 0;
	pos_comp.mDrawArea.top = 0;
	pos_comp.mDrawArea.right = 300;
	pos_comp.mDrawArea.bottom = 300;
	unit_reader_comp.playerID = 0;
	unit_reader_comp.unitPlacement = UIUnitReader::UI_UNIT_RIGHT;
	rECS.createEntity(bitmap_comp, pos_comp, unit_reader_comp);//top left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/RightMan.png", "RightMan");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 0, 0);
	pos_comp.mDrawArea.left = width - 300;
	pos_comp.mDrawArea.top = 0;
	pos_comp.mDrawArea.right = width;
	pos_comp.mDrawArea.bottom = 300;
	unit_reader_comp.playerID = 1;
	unit_reader_comp.unitPlacement = UIUnitReader::UI_UNIT_RIGHT;
	rECS.createEntity(bitmap_comp, pos_comp, unit_reader_comp); //top right

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/RightMan.png", "RightMan");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 0, 0);
	pos_comp.mDrawArea.left = 0;
	pos_comp.mDrawArea.top = height - 300;
	pos_comp.mDrawArea.right = 300;
	pos_comp.mDrawArea.bottom = height;
	unit_reader_comp.playerID = 2;
	unit_reader_comp.unitPlacement = UIUnitReader::UI_UNIT_RIGHT;
	rECS.createEntity(bitmap_comp, pos_comp, unit_reader_comp);//bottom left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/RightMan.png", "RightMan");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 0, 0);
	pos_comp.mDrawArea.left = width - 300;
	pos_comp.mDrawArea.top = height - 300;
	pos_comp.mDrawArea.right = width;
	pos_comp.mDrawArea.bottom = height;
	unit_reader_comp.playerID = 3;
	unit_reader_comp.unitPlacement = UIUnitReader::UI_UNIT_RIGHT;
	rECS.createEntity(bitmap_comp, pos_comp, unit_reader_comp);//bottom right

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/MiddleMan.png", "MiddleMan");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 0, 0);
	pos_comp.mDrawArea.left = 0;
	pos_comp.mDrawArea.top = 0;
	pos_comp.mDrawArea.right = 300;
	pos_comp.mDrawArea.bottom = 300;
	unit_reader_comp.playerID = 0;
	unit_reader_comp.unitPlacement = UIUnitReader::UI_UNIT_MIDDLE;
	rECS.createEntity(bitmap_comp, pos_comp, unit_reader_comp);//top left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/MiddleMan.png", "MiddleMan");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 0, 0);
	pos_comp.mDrawArea.left = width - 300;
	pos_comp.mDrawArea.top = 0;
	pos_comp.mDrawArea.right = width;
	pos_comp.mDrawArea.bottom = 300;
	unit_reader_comp.playerID = 1;
	unit_reader_comp.unitPlacement = UIUnitReader::UI_UNIT_MIDDLE;
	rECS.createEntity(bitmap_comp, pos_comp, unit_reader_comp); //top right

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/MiddleMan.png", "MiddleMan");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 0, 0);
	pos_comp.mDrawArea.left = 0;
	pos_comp.mDrawArea.top = height - 300;
	pos_comp.mDrawArea.right = 300;
	pos_comp.mDrawArea.bottom = height;
	unit_reader_comp.playerID = 2;
	unit_reader_comp.unitPlacement = UIUnitReader::UI_UNIT_MIDDLE;
	rECS.createEntity(bitmap_comp, pos_comp, unit_reader_comp);//bottom left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/MiddleMan.png", "MiddleMan");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 0, 0);
	pos_comp.mDrawArea.left = width - 300;
	pos_comp.mDrawArea.top = height - 300;
	pos_comp.mDrawArea.right = width;
	pos_comp.mDrawArea.bottom = height;
	unit_reader_comp.playerID = 3;
	unit_reader_comp.unitPlacement = UIUnitReader::UI_UNIT_MIDDLE;
	rECS.createEntity(bitmap_comp, pos_comp, unit_reader_comp);//bottom right

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/Nametag.png", "Nametag");
	bitmap_comp.to_draw = true;
	bitmap_comp.mpTintedBitmap = nullptr;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, NULL, 200, 200, 200);
	pos_comp.mDrawArea.left = 20;
	pos_comp.mDrawArea.top = 230;
	pos_comp.mDrawArea.right = 280;
	pos_comp.mDrawArea.bottom = 300;
	rECS.createEntity(bitmap_comp, pos_comp);//top left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/Nametag.png", "Nametag");
	bitmap_comp.mpTintedBitmap = nullptr;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, NULL, 200, 200, 200);
	pos_comp.mDrawArea.left = width - 280;
	pos_comp.mDrawArea.top = 230;
	pos_comp.mDrawArea.right = width - 20;
	pos_comp.mDrawArea.bottom = 300;
	rECS.createEntity(bitmap_comp, pos_comp);//top right

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/Nametag.png", "Nametag");
	bitmap_comp.mpTintedBitmap = nullptr;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, NULL, 200, 200, 200);
	pos_comp.mDrawArea.left = 20;
	pos_comp.mDrawArea.top = height - 70;
	pos_comp.mDrawArea.right = 280;
	pos_comp.mDrawArea.bottom = height;
	rECS.createEntity(bitmap_comp, pos_comp);//bottom left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/Nametag.png", "Nametag");
	bitmap_comp.mpTintedBitmap = nullptr;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, NULL, 200, 200, 200);
	pos_comp.mDrawArea.left = width - 280;
	pos_comp.mDrawArea.top = height - 70;
	pos_comp.mDrawArea.right = width - 20;
	pos_comp.mDrawArea.bottom = height;
	rECS.createEntity(bitmap_comp, pos_comp);//bottom right

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/LeftStar.png", "LeftStar");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	bitmap_comp.mBitmapID = 0;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 255, 255);
	pos_comp.mDrawArea.left = 40;
	pos_comp.mDrawArea.top = 0;
	pos_comp.mDrawArea.right = 250;
	pos_comp.mDrawArea.bottom = 100;
	army_reader_comp.playerID = 0;
	rECS.createEntity(bitmap_comp, pos_comp, army_reader_comp);//top left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/LeftStar.png", "LeftStar");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	bitmap_comp.mBitmapID = 0;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 255, 255);
	pos_comp.mDrawArea.left = width - 250;
	pos_comp.mDrawArea.top = 0;
	pos_comp.mDrawArea.right = width - 40;
	pos_comp.mDrawArea.bottom = 100;
	army_reader_comp.playerID = 1;
	rECS.createEntity(bitmap_comp, pos_comp, army_reader_comp);//top right

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/LeftStar.png", "LeftStar");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	bitmap_comp.mBitmapID = 0;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 255, 255);
	pos_comp.mDrawArea.left = 40;
	pos_comp.mDrawArea.top = height - 300;
	pos_comp.mDrawArea.right = 250;
	pos_comp.mDrawArea.bottom = height - 200;
	army_reader_comp.playerID = 2;
	rECS.createEntity(bitmap_comp, pos_comp, army_reader_comp);//bottom left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/LeftStar.png", "LeftStar");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	bitmap_comp.mBitmapID = 0;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 255, 255);
	pos_comp.mDrawArea.left = width - 250;
	pos_comp.mDrawArea.top = height - 300;
	pos_comp.mDrawArea.right = width - 40;
	pos_comp.mDrawArea.bottom = height - 200;
	army_reader_comp.playerID = 3;
	rECS.createEntity(bitmap_comp, pos_comp, army_reader_comp);//bottom right

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/MiddleStar.png", "MiddleStar");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	bitmap_comp.mBitmapID = 1;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 255, 255);
	pos_comp.mDrawArea.left = 40;
	pos_comp.mDrawArea.top = 0;
	pos_comp.mDrawArea.right = 250;
	pos_comp.mDrawArea.bottom = 100;
	army_reader_comp.playerID = 0;
	rECS.createEntity(bitmap_comp, pos_comp, army_reader_comp);//top left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/MiddleStar.png", "MiddleStar");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	bitmap_comp.mBitmapID = 1;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 255, 255);
	pos_comp.mDrawArea.left = width - 250;
	pos_comp.mDrawArea.top = 0;
	pos_comp.mDrawArea.right = width - 40;
	pos_comp.mDrawArea.bottom = 100;
	army_reader_comp.playerID = 1;
	rECS.createEntity(bitmap_comp, pos_comp, army_reader_comp);//top right

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/MiddleStar.png", "MiddleStar");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	bitmap_comp.mBitmapID = 1;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 255, 255);
	pos_comp.mDrawArea.left = 40;
	pos_comp.mDrawArea.top = height - 300;
	pos_comp.mDrawArea.right = 250;
	pos_comp.mDrawArea.bottom = height - 200;
	army_reader_comp.playerID = 2;
	rECS.createEntity(bitmap_comp, pos_comp, army_reader_comp);//bottom left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/MiddleStar.png", "MiddleStar");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	bitmap_comp.mBitmapID = 1;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 255, 255);
	pos_comp.mDrawArea.left = width - 250;
	pos_comp.mDrawArea.top = height - 300;
	pos_comp.mDrawArea.right = width - 40;
	pos_comp.mDrawArea.bottom = height - 200;
	army_reader_comp.playerID = 3;
	rECS.createEntity(bitmap_comp, pos_comp, army_reader_comp);//bottom right

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/RightStar.png", "RightStar");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	bitmap_comp.mBitmapID = 2;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 255, 255);
	pos_comp.mDrawArea.left = 40;
	pos_comp.mDrawArea.top = 0;
	pos_comp.mDrawArea.right = 250;
	pos_comp.mDrawArea.bottom = 100;
	army_reader_comp.playerID = 0;
	rECS.createEntity(bitmap_comp, pos_comp, army_reader_comp);//top left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/RightStar.png", "RightStar");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	bitmap_comp.mBitmapID = 2;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 255, 255);
	pos_comp.mDrawArea.left = width - 250;
	pos_comp.mDrawArea.top = 0;
	pos_comp.mDrawArea.right = width - 40;
	pos_comp.mDrawArea.bottom = 100;
	army_reader_comp.playerID = 1;
	rECS.createEntity(bitmap_comp, pos_comp, army_reader_comp);//top right

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/RightStar.png", "RightStar");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	bitmap_comp.mBitmapID = 2;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 255, 255);
	pos_comp.mDrawArea.left = 40;
	pos_comp.mDrawArea.top = height - 300;
	pos_comp.mDrawArea.right = 250;
	pos_comp.mDrawArea.bottom = height - 200;
	army_reader_comp.playerID = 2;
	rECS.createEntity(bitmap_comp, pos_comp, army_reader_comp);//bottom left

	bitmap_comp.mpBitmap = d2d->LoadImageToBitmap("../../UI/Resource/RightStar.png", "RightStar");
	bitmap_comp.mpTintedBitmap = d2d->CreateBitmapTarget(bitmap_comp.mpBitmap->GetSize().width, bitmap_comp.mpBitmap->GetSize().height);
	bitmap_comp.mBitmapID = 2;
	d2d->SetBitmapTint(bitmap_comp.mpBitmap, bitmap_comp.mpTintedBitmap, 255, 255, 255);
	pos_comp.mDrawArea.left = width - 250;
	pos_comp.mDrawArea.top = height - 300;
	pos_comp.mDrawArea.right = width - 40;
	pos_comp.mDrawArea.bottom = height - 200;
	army_reader_comp.playerID = 3;
	rECS.createEntity(bitmap_comp, pos_comp, army_reader_comp);//bottom right

	rECS.createSystem<ecs::systems::UIOverlayInitSystem>(4)->mpD2D = d2d;
	rECS.createSystem<ecs::systems::UIUnitColorUpdateSystem>(7)->mpD2D = d2d;
}
