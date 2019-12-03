#include "UpdateOceanSystem.h"

#include "../gameUtility/UtilityComponents.h"
#include "../gameWorld/WorldComponents.h"

using namespace DirectX;

namespace ecs
{
	namespace systems
	{
		UpdateOceanSystem::UpdateOceanSystem()
		{
			updateType = Actor;

			mOceanCenter = { 0.f, 0.f, 0.f };
			mCycleDuration = 0.f;
		}

		UpdateOceanSystem::~UpdateOceanSystem()
		{
			//
		}

		bool UpdateOceanSystem::Initialize(float cycleDuration)
		{
			typeFilter.addRequirement(components::TransformComponent::typeID);
			typeFilter.addRequirement(components::OceanTileComponent::typeID);
			typeFilter.addRequirement(components::ColorComponent::typeID);

			mOceanTiles = getEntitiesByFilter(typeFilter);

			if (!mOceanTiles.entities.size())
			{
				return false;
			}

			XMVECTOR center = { 0.f, 0.f, 0.f };
			XMFLOAT3 position;
			for (FilteredEntity& tile : mOceanTiles.entities)
			{
				position = tile.getComponent<components::TransformComponent>()->position;
				position.y = 0.f;
				center += XMLoadFloat3(&position);
			}
			center /= (float)mOceanTiles.entities.size();

			XMStoreFloat3(&mOceanCenter, center);
			mCycleDuration = cycleDuration;
			return true;
		}

		void UpdateOceanSystem::act(float _delta)
		{
			mTimeElapsed += _delta;

			//while (mTimeElapsed > mCycleDuration)
			//{
			//	mTimeElapsed -= mCycleDuration;
			//}

			XMFLOAT3 position;
			float distance_to_center;
			components::TransformComponent* p_transform;
			for (FilteredEntity& tile : mOceanTiles.entities)
			{
				//components::ColorComponent* p_color = tile.getComponent<

				p_transform = tile.getComponent<components::TransformComponent>();

				position = p_transform->position;
				position.y = 0.f;

				distance_to_center = XMVectorGetX(XMVector3Length(XMLoadFloat3(&position) - XMLoadFloat3(&mOceanCenter)));

				p_transform->position.y = sin(mTimeElapsed * distance_to_center * 0.2f) * 0.1f - 0.8f;
			}
		}
	}
}