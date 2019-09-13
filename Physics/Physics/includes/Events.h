#pragma once
#include "ecs.h"

#define EVENT(name) struct name : public ecs::ECSEvent<name>

enum MovementKeys
{
	W, A, S, D
};

EVENT(MovementInputEvent)
{
	// TODO: Hook into input handler.
	MovementKeys key;
};
