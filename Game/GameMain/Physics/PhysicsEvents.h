#pragma once
#include "ecs.h"
#include "MovementFunctions.h"
#define EVENT(name) struct name : public ecs::ECSEvent<name>

namespace ecs
{
	namespace events
	{
		// MovementInputs are used to move entities in various directions.
		//enum MovementInputs
		//{
		//	FORWARD		= 0, 
		//	RIGHT		= 90, 
		//	LEFT		= -90, 
		//	BACKWARD	= 180
		//};
		
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

	} // events
} // ecs
