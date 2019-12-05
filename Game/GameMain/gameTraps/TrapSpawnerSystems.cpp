#include "TrapSpawnerSystems.h"
#include "TrapComponents.h"
#include "TrapEvents.h"

#include "../gameUtility/UtilityComponents.h"
#include "../gameUtility/UtilityGraphics.h"

using namespace DirectX;

#include <iostream>

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
			if (_event.getTypeID() != events::PlaceTrapEvent::typeID)
			{
				return;
			}

			events::PlaceTrapEvent& r_trap_event = static_cast<events::PlaceTrapEvent&>(_event);
			components::TransformComponent* p_tile_transf = getComponentFromKnownEntity<components::TransformComponent>(r_trap_event.tileID);

			/*
				-- Sanity check the event's trap type input
				Never create a trap that isn't defined.
			*/

			if (r_trap_event.type <= GAME_OBJECT_TYPE_TRAP_OFFSET_TAG || r_trap_event.type > GAME_OBJECT_TYPE(GAME_OBJECT_TYPE_TRAP_OFFSET_TAG + TRAP_TYPE_COUNT))
			{
				return;
			}

			/*
				-- Create dummy components
				Used to initialize trap entity components.
			*/

			components::TrapComponent trap_comp;
			components::ColorComponent color_comp;
			components::TransformComponent transf_comp;

			/*
				-- Set trap type and tileID
			*/

			trap_comp.mObjectType = r_trap_event.type;
			trap_comp.mTileID = r_trap_event.tileID;

			trap_comp.ActivationRateInSeconds = 4.0f + ((rand() % 2));
			trap_comp.CurrentTimeInSeconds = 0.0f;

			/*
				-- Set trap position
				Trap is located above the tile it's created on. Add an
				offset to be placed above it.
			*/

			transf_comp.position = p_tile_transf->position;
			transf_comp.position.y += 0.01f;

			transf_comp.scale.x = 0.9;
			transf_comp.scale.y = 0.9;
			transf_comp.scale.z = 0.9;

			/*
				-- Set trap color
				For now, all traps use the same mesh. Only difference
				is the color of the trap.
			*/	

			switch (trap_comp.mObjectType)
			{
			case GAME_OBJECT_TYPE_TRAP_FIRE:
				color_comp = components::ColorComponent(40, 40, 40);
				break;

			case GAME_OBJECT_TYPE_TRAP_FREEZE:
				color_comp = components::ColorComponent(150, 150, 150);
				break;

			case GAME_OBJECT_TYPE_TRAP_SPRING:
				color_comp = components::ColorComponent(191, 128, 64);
				break;

			case GAME_OBJECT_TYPE_TRAP_SPIKES:
				color_comp = components::ColorComponent(50, 50, 50);
				transf_comp.position.y -= 10.0f;
				break;

			default:
				color_comp = components::ColorComponent(); // Default constructor sets color to black.
				break;
			}


			/*
				-- Create trap BV
				Creates an AABB around the trap mesh for collision.
			*/
			std::vector<XMFLOAT3>* p_vertices = MeshContainer::GetMeshCPU(trap_comp.mObjectType)->GetVertexPositionVector();
			trap_comp.mAABB.CreateFromPoints(trap_comp.mAABB, p_vertices->size(), p_vertices->data(), sizeof(XMFLOAT3));

			// Boost height of spring BV slightly since it's a circle.
			if (trap_comp.mObjectType == GAME_OBJECT_TYPE_TRAP_SPRING)
			{
				trap_comp.mAABB.Extents.y += 0.1f;
			}

			createEntity(trap_comp, transf_comp, color_comp);
		}
	}
}