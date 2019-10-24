#pragma once
#include "ecs.h"
#include "Direct2D.h"
#include "UISystems.h"

struct TempUISystemPtrs
{
	ecs::systems::UIPreRenderSystem* UIpreSys;
	ecs::systems::UIBitmapSystem* UIBitmapSys;
	ecs::systems::UITextSystem* UITextSys;
	ecs::systems::UIDebugSystem* UIDebugSys;
	ecs::systems::UIPostRenderSystem* UIpostSys;
	//ecs::systems::UIRectSystem* UIrectSys;
};

void InitUI(ecs::EntityComponentSystem& rECS, Direct2D** D2D, TempUISystemPtrs& rSystemPointers)
{
	rSystemPointers.UIpreSys	= rECS.createSystem<ecs::systems::UIPreRenderSystem>(7);
	rSystemPointers.UIBitmapSys = rECS.createSystem<ecs::systems::UIBitmapSystem>(8);
	rSystemPointers.UITextSys	= rECS.createSystem<ecs::systems::UITextSystem>(8);
	//rSystemPointers.UIrectSys	= rECS.createSystem<ecs::systems::UIRectSystem>(8);
	rSystemPointers.UIDebugSys	= rECS.createSystem<ecs::systems::UIDebugSystem>(8);
	rSystemPointers.UIpostSys	= rECS.createSystem<ecs::systems::UIPostRenderSystem>(9);
	//*D2D = new Direct2D;
	rSystemPointers.UIpreSys->mpD2D			= 
		rSystemPointers.UITextSys->mpD2D	= 
		rSystemPointers.UIpostSys->mpD2D	= 
		rSystemPointers.UIBitmapSys->mpD2D  = 
		rSystemPointers.UIDebugSys->mpD2D	= *D2D;


	ecs::components::UITextComponent text_comp;
	text_comp.mStrText = "adkguhadigsdgkasgd";
	ecs::components::UIDrawPosComponent pos_comp;
	pos_comp.mDrawArea.top = 100;
	pos_comp.mDrawArea.bottom = 200;
	pos_comp.mDrawArea.left = 100;
	pos_comp.mDrawArea.right= 200;
	ecs::components::UIDrawColorComponent color_comp;
	color_comp.mColor = brushColors::Red;


	rECS.createEntity(text_comp, pos_comp, color_comp);

	ecs::components::UIThicknessComponent thick_comp;
	thick_comp.mThickness = 0;

	rECS.createEntity(thick_comp, pos_comp, color_comp);

}