#include "WorldScenerySystems.h"
#include "WorldComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../Physics/PhysicsComponents.h"
#include "../Physics/PhysicsEvents.h"

#include "../gameAI/AIComponents.h"


namespace ecs
{
	namespace systems
	{
		WorldSceneryUpdateSystem::WorldSceneryUpdateSystem()
		{
			updateType = EntityUpdate;
			typeFilter.addRequirement(components::WorldSceneryComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
			typeFilter.addRequirement(components::ColorComponent::typeID);
		}

		WorldSceneryUpdateSystem::~WorldSceneryUpdateSystem()
		{
			//
		}

		void WorldSceneryUpdateSystem::Initialize()
		{
			TypeFilter map_filter;
			map_filter.addRequirement(components::TileComponent::typeID);
			map_filter.addRequirement(components::TransformComponent::typeID);
			EntityIterator tiles = getEntitiesByFilter(map_filter);

			int valid_tiles = 0;
			XMVECTOR center = { 0.f, 0.f, 0.f, 0.f };
			for (FilteredEntity tile : tiles.entities)
			{
				if (tile.getComponent<components::TileComponent>()->tileType != WATER)
				{
					center += XMLoadFloat3(&tile.getComponent<components::TransformComponent>()->position);
					valid_tiles++;
				}
			}
			center /= (float)valid_tiles;
			XMStoreFloat3(&mCenter, center);
		}

		void WorldSceneryUpdateSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
		{
			components::DynamicMovementComponent* dyn_move_comp = getComponentFromKnownEntity<components::DynamicMovementComponent>(_entityInfo.entity->getID());
			components::TransformComponent* p_transform = _entityInfo.getComponent<components::TransformComponent>();

			XMVECTOR center_pos = XMLoadFloat3(&this->mCenter);
			XMVECTOR shark_pos = XMLoadFloat3(&p_transform->position);
			XMVECTOR center_to_shark_normalized = XMVector3Normalize(shark_pos - center_pos);// Normalized vector from center of map to shark

			XMVECTOR pre_forward_vector;
			XMVECTOR post_forward_vector;
			XMVECTOR rotation_vector;

			pre_forward_vector = XMVector3Cross(center_to_shark_normalized, { 0.f,1.f,0.f,0.f }); //Get new forward vector from the vectors center to shark and up vector

			shark_pos += pre_forward_vector * 5.f * _delta;
			center_to_shark_normalized = XMVector3Normalize(shark_pos - center_pos);
			shark_pos = center_pos + center_to_shark_normalized * m_MAP_RADIOUS;

			post_forward_vector = XMVector3Cross(center_to_shark_normalized, { 0.f,1.f,0.f,0.f });

			rotation_vector = XMVector3AngleBetweenNormals(pre_forward_vector, post_forward_vector); //Get rotation from old forward and new forward

			XMStoreFloat3(&p_transform->position, shark_pos);
			p_transform->rotation.y = Sign(XMVectorGetX(post_forward_vector)) * acosf(XMVectorGetX(XMVector3Dot(post_forward_vector, { 0.f, 0.f, 1.f, 0.f }))); // Rotate the shark
			p_transform->rotation.y -= 3.14f / 2.f;

		}
	}
}