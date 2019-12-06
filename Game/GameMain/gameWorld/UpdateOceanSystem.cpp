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
		constexpr int WAVESCOUNT = 100;

		UpdateOceanSystem::UpdateOceanSystem()
		{
			updateType = Actor;

			mOceanCenter = { 0.f, 0.f, 0.f };
			mCycleDuration = 0.f;

		}

		UpdateOceanSystem::~UpdateOceanSystem()
		{
			delete mWaveArray;
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



			mWaveArray = new float[WAVESCOUNT];
			for (size_t i = 0; i < WAVESCOUNT; i++)
				mWaveArray[i] = -1.0f;
			
			mWaveArray[0] = 20.0f;

			mFlatternOutFactor = 0.9f;
			mUpdateIntervall = 0.01f;
			mTimeElapsed = 0.0f;

			// Create a entity with one component pointing at the first element in mWaveArray
			components::WaveCenterComponent wave;
			wave.mpFistElement = &mWaveArray[0];
			createEntity(wave);

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

				const float MAXDIST = OCEAN_RADIUS + 10.0f;

				//unsigned int index_to_pick = rand() % WAVESCOUNT;
				unsigned int index_to_pick = (unsigned int)(WAVESCOUNT * distance_to_center / MAXDIST);

				if (index_to_pick >= WAVESCOUNT)
					index_to_pick = WAVESCOUNT - 1;

				p_transform->position.y = mWaveArray[index_to_pick];

				
				//Globeish
				//p_transform->position.y = (mWaveArray[index_to_pick] * ((distance_to_center * (distance_to_center / 2.0f)) / MAXDIST)) + 0.2f;
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
						*p_wave->mpFistElement = 20.0f;
					};
				}
			}
			
			

			// Have the update depend on delta
			if (mTimeElapsed > mUpdateIntervall)
			{
				// To make sure they arnt the same
				mTimeElapsed += 0.01f;

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