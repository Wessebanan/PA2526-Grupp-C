#include "TrapSpawnerSystems.h"
#include "TrapComponents.h"
#include "TrapEvents.h"

#include "../gameUtility/UtilityComponents.h"
#include "../gameUtility/UtilityGraphics.h"

using namespace DirectX;

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
			components::TransformComponent* p_tile_transf = getComponentFromKnownEntity<components::TransformComponent>(r_trap_event.tileID);

			/*
				Create dummy components, used to initialize trap entity components.
			*/

			components::TrapComponent trap_comp;
			components::ColorComponent color_comp;
			components::TransformComponent transf_com;

			/*
				-- Set trap type
			*/

			trap_comp.mObjectType = r_trap_event.type;

			/*
				-- Set trap position
				Trap is located above the tile it's created on. Add an
				offset to be placed above it.
			*/
			XMVECTOR position_offset = { 0.f, 0.1f, 0.f, 0.f };
			XMStoreFloat3(&transf_com.position, XMLoadFloat3(&p_tile_transf->position) + position_offset);

			/*
				-- Set trap color
				For now, all traps use the same mesh. Only difference
				is the color of the trap.
			*/	

			switch (trap_comp.mObjectType)
			{
			case GAME_OBJECT_TYPE_TRAP_FIRE:
				color_comp = components::ColorComponent(255, 0, 0);
				break;

			case GAME_OBJECT_TYPE_TRAP_FREEZE:
				color_comp = components::ColorComponent(200, 200, 255);
				break;

			case GAME_OBJECT_TYPE_TRAP_SPRING:
				color_comp = components::ColorComponent(191, 128, 64);
				break;

			default:
				color_comp = components::ColorComponent(); // Default constructor sets color to black.
				break;
			}

			createEntity(trap_comp, transf_com, color_comp);
		}
	}
}