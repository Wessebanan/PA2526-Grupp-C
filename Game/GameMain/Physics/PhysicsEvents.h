#pragma once
#include "ecs.h"
#include "MovementFunctions.h"
#include "AIGlobals.h"

#define EVENT(name) struct name : public ecs::ECSEvent<name>

namespace ecs
{
	namespace events
	{
		// This event should be created
		// whenever a movement action is performed
		// in order to alert the movement systems.
		EVENT(MovementInputEvent)
		{
			MovementInputs mInput;
		
			// The entity that the user wants to move.
			// Entity needs to support static movement.
			ID mEntityID;			
		};

		// Used to notify collision systems that 
		// an entity with a bounding volume has moved.
		EVENT(PotentialCollisionEvent)
		{
			ID mEntityID;
			float mDelta;
		};

		// Used to apply general forces to events that are uncapped.
		EVENT(ForceImpulseEvent)
		{
			ID mEntityID;
			XMFLOAT3 mDirection;
			float mForce;
		};

		// Did not know where to put this haha.
		EVENT(ColorSwitchEvent)
		{
			Color mColor;
			ID mEntityID;
			float mTime;
		};
	} // events
} // ecs
