#pragma once
#include "ecs.h"
#include "Events.h"
#include <Windows.h>
#define SYSTEM(name) struct name : public ecs::ECSSystem<name>

// Temporary system to test movement input events.
SYSTEM(InputSystem)
{
	InputSystem();
	virtual ~InputSystem();
	void updateEntity(ecs::FilteredEntity & _entityInfo, float _delta) override;
};