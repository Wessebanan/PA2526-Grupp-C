#pragma once
//#include "ecs.h"
#include "Components.h"
#include "Events.h"
#include "Fbx_Loader.h"
#include "CameraSystems.h"
#include "DebugInfo.h"

#define SYSTEM(name) struct name : public ecs::ECSSystem<name>

SYSTEM(StaticMovementSystem)
{
	// TODO: Handle input from input handler.
	StaticMovementSystem();
	
	virtual ~StaticMovementSystem();

	void updateEntity(ecs::FilteredEntity & _entityInfo, float _delta) override;
};