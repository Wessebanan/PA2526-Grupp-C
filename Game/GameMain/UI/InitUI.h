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
};

void InitUI(ecs::EntityComponentSystem& rECS, Direct2D** D2D, TempUISystemPtrs& rSystemPointers)
{
	rSystemPointers.UIpreSys	= rECS.createSystem<ecs::systems::UIPreRenderSystem>(0);
	rSystemPointers.UIBitmapSys = rECS.createSystem<ecs::systems::UIBitmapSystem>();
	rSystemPointers.UITextSys	= rECS.createSystem<ecs::systems::UITextSystem>();
	rSystemPointers.UIDebugSys	= rECS.createSystem<ecs::systems::UIDebugSystem>(5);
	//rSystemPointers.UIpostSys	= rECS.createSystem<ecs::systems::UIPostRenderSystem>(6);
	*D2D = new Direct2D;
	rSystemPointers.UIpreSys->mpD2D			= 
		rSystemPointers.UITextSys->mpD2D	= 
		//rSystemPointers.UIpostSys->mpD2D	= 
		rSystemPointers.UIBitmapSys->mpD2D  = 
		rSystemPointers.UIDebugSys->mpD2D	= *D2D;
}