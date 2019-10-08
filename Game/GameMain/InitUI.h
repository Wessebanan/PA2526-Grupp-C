#pragma once
#include "ecs.h"
#include "UISystems.h"
#include "Direct2D.h"

void InitUI(ecs::EntityComponentSystem& rECS, Direct2D** D2D)
{
	ecs::systems::UIPreRenderSystem* UIpreSys = rECS.createSystem<ecs::systems::UIPreRenderSystem>(0);
	ecs::systems::UIBitmapSystem* UIBitmapSys = rECS.createSystem<ecs::systems::UIBitmapSystem>();
	ecs::systems::UITextSystem* UITextSys = rECS.createSystem<ecs::systems::UITextSystem>();
	ecs::systems::UIDebugSystem* UIDebugSys = rECS.createSystem<ecs::systems::UIDebugSystem>(9);
	ecs::systems::UIPostRenderSystem* UIpostSys = rECS.createSystem<ecs::systems::UIPostRenderSystem>(9);
	*D2D = new Direct2D;
	UIpreSys->mpD2D = UITextSys->mpD2D = UIpostSys->mpD2D = UIBitmapSys->mpD2D = UIDebugSys->mpD2D = *D2D;
}