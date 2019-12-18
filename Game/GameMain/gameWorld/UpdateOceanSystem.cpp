#include "UpdateOceanSystem.h"

#include "../gameUtility/UtilityComponents.h"
#include "../gameAI/AIComponents.h"
#include "../gameWorld/WorldComponents.h"

#include "WorldSettings.h"

#include "../Input/InitInputComponents.h"

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
			int counter = 0;

			ComponentIterator itt = getComponentsOfType<components::TileComponent>();
			components::TileComponent* p_tile_comp;
			while (p_tile_comp = (components::TileComponent*)itt.next())
			{
				if (p_tile_comp->tileType == TileTypes::GAME_FIELD)
				{
					position = getComponentFromKnownEntity<components::TransformComponent>(p_tile_comp->getEntityID())->position;
					position.y = 0.f;
					center += XMLoadFloat3(&position);
					counter++;
				}
			}
			center /= counter;

			XMStoreFloat3(&mOceanCenter, center);
			mCycleDuration = cycleDuration;


			for (size_t i = 0; i < WAVESCOUNT; i++)
				mWaveArray[i] = 0.0f;
			
			mWaveArray[0] = 20.0f;

			mFlatternOutFactor = 0.999f;
			mUpdateIntervall = 0.005f;
			mTimeElapsed = 0.0f;

			// Create a entity with one component pointing at the first element in mWaveArray
			components::WaveCenterComponent wave;
			wave.mpFirstElement = &mWaveArray[0];
			createEntity(wave);

			return true;
		}

		void UpdateOceanSystem::act(float _delta)
		{
			mTimeElapsed += _delta;

			XMFLOAT3 position;
			float distance_to_center;
			components::TransformComponent* p_transform;
			components::OceanTileComponent* p_ocean;
			for (FilteredEntity& tile : mOceanTiles.entities)
			{
				p_transform = tile.getComponent<components::TransformComponent>();
				p_ocean = tile.getComponent<components::OceanTileComponent>();

				if (p_ocean->mWaveIndex > WAVESCOUNT)
				{
					position = p_transform->position;
					position.y = 0.f;
					mOceanCenter.y = 0.f;

					distance_to_center = XMVectorGetX(XMVector3Length(XMLoadFloat3(&position) - XMLoadFloat3(&mOceanCenter)));

					const float MAXDIST = OCEAN_RADIUS + 10.0f;

					unsigned int index_to_pick = (unsigned int)(WAVESCOUNT * distance_to_center / MAXDIST);

					unsigned int offset = 15;

					if (index_to_pick >= WAVESCOUNT)
						index_to_pick = WAVESCOUNT - 1;
					else if (index_to_pick < offset)
						index_to_pick = offset;

					p_ocean->mWaveIndex = index_to_pick - offset;
					
					p_ocean->mWaveAmplifier = ((distance_to_center/* * (distance_to_center*0.1f)*/) / MAXDIST) * 1.7f;
				}
				
				p_transform->position.y = (mWaveArray[p_ocean->mWaveIndex] * p_ocean->mWaveAmplifier);

				// Lower the water level
				p_transform->position.y -= 1.5f;
			}

			// to be change to music waves
			ComponentIterator itt = getComponentsOfType<components::KeyboardComponent>();
			components::KeyboardComponent* p_key_comp;
			if (p_key_comp = (components::KeyboardComponent*)itt.next())
			{
				if (p_key_comp->R)
				{
					//mWaveArray[0] = 20.0f
					itt = getComponentsOfType<components::WaveCenterComponent>();
					components::WaveCenterComponent* p_wave;
					if (p_wave = (components::WaveCenterComponent*)itt.next())
					{
						*p_wave->mpFirstElement = 5.0f;
					};
				}
			}

			// Have the update depend on delta
			if (mTimeElapsed > mUpdateIntervall)
			{
				// To make sure they arnt the same
				mTimeElapsed += 0.0001f;

				// To make sure it will keep up
				for (size_t j = 0; j < (int)((mTimeElapsed/mUpdateIntervall) - 1); j++)
				{
					// Moves the waves forward
					mWaveArray[0] *= mFlatternOutFactor/* / _delta*/;
					for (size_t i = WAVESCOUNT- 2; i >= 2; i--)
					{
						float height = 0.0f;
						height += mWaveArray[i - 2];
						height += mWaveArray[i - 1];
						height += mWaveArray[i];
						height += mWaveArray[i + 1];

						height /= 4;

						mWaveArray[i] = height;
					}
				}
				
				// reset time
				mTimeElapsed = 0.0f;
			}
			
		}
	}
}