#pragma once
#include "ecs.h"
#include "Events.h"
#include "Components.h"
#include "UtilityComponents.h"
#define SYSTEM(name) class name : public ecs::ECSSystem<name>


namespace ecs
{
	namespace systems
	{
	
		// Temporary system to test movement input events.
		SYSTEM(InputSystem)
		{
		public:
			InputSystem();
			virtual ~InputSystem();
			void updateEntity(ecs::FilteredEntity & _entityInfo, float _delta) override;
		};
	} // systems
} // ecs