#include "TrapSpawnerSystems.h"
#include "TrapComponents.h"
#include "TrapEvents.h"

#include "../gameUtility/UtilityComponents.h"

namespace ecs
{
	namespace systems
	{
		TrapSpawnerSystem::TrapSpawnerSystem()
		{
			updateType = EventReader;
			typeFilter.addRequirement(events::PlaceTrapEvent::typeID);
		}

		TrapSpawnerSystem::~TrapSpawnerSystem()
		{
			//
		}

		void TrapSpawnerSystem::readEvent(BaseEvent& _event, float _delta)
		{
			events::PlaceTrapEvent& r_trap_event = static_cast<events::PlaceTrapEvent&>(_event);
			components::TransformComponent* p_transf_comp = getComponentFromKnownEntity<components::TransformComponent>(r_trap_event.tileID);

			// TODO
		}
	}
}