#include "Renderers.h"
#include "../gameAI/AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "../gameUtility/UtilityEcsFunctions.h"
#include "../gameSceneObjects/SceneObjectComponents.h"

inline uint32_t PACK(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3) {
	return (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;
}

namespace ecs
{
	namespace systems
	{
#pragma region UnitRenderSystem
		UnitRenderSystem::UnitRenderSystem() : mFrameCounter(0), mAnimationFrameCounter(0)
		{
			updateType = SystemUpdateType::MultiEntityUpdate;
			typeFilter.addRequirement(components::UnitComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
			//typeFilter.addRequirement(components::Color::typeID);

			mpSkeleton = MeshContainer::GetMesh(MESH_TYPE_UNIT)->GetSkeleton();
		}

		UnitRenderSystem::~UnitRenderSystem()
		{

		}

		void UnitRenderSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
		{
			int index = 0;
			for (FilteredEntity unit : _entities.entities)
			{
				components::TransformComponent* p_transform_comp = unit.getComponent<ecs::components::TransformComponent>();
				DirectX::XMMATRIX world = UtilityEcsFunctions::GetWorldMatrix(*p_transform_comp);

				XMStoreFloat4x4(&mpBuffer[index].world, world);

				memcpy(
					mpBuffer[index].boneMatrices,
					&mpSkeleton->animationData[mAnimationFrameCounter * mpSkeleton->jointCount],
					mpSkeleton->jointCount * sizeof(DirectX::XMFLOAT4X4));

				index++;
			}

			mFrameCounter++;
			if (mFrameCounter % 10 == 0)
			{
				mFrameCounter = 0;
				mAnimationFrameCounter = ++mAnimationFrameCounter % mpSkeleton->frameCount;
			}
		}

		void UnitRenderSystem::SetBegin(void* pBufferStart)
		{
			mpBuffer = (InputLayout*)pBufferStart;
		}
#pragma endregion UnitRenderSystem

#pragma region TileRenderSystem
		TileRenderSystem::TileRenderSystem()
		{
			updateType = SystemUpdateType::MultiEntityUpdate;
			typeFilter.addRequirement(components::TileComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
			typeFilter.addRequirement(components::ColorComponent::typeID);
		}

		TileRenderSystem::~TileRenderSystem()
		{

		}

		void TileRenderSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
		{
			uint32_t index = 0;
			for (FilteredEntity tile: _entities.entities)
			{

				components::TileComponent* p_tile_comp = tile.getComponent<components::TileComponent>();
				components::TransformComponent* p_transform_comp = tile.getComponent<components::TransformComponent>();
				components::ColorComponent* p_color_comp = tile.getComponent<components::ColorComponent>();

				mpBuffer[index].x = p_transform_comp->position.x;
				mpBuffer[index].y = p_transform_comp->position.y;
				mpBuffer[index].z = p_transform_comp->position.z;

				uint32_t random = (uint32_t)rand() % 101;
				uint32_t color_offset = -50 + random;
				switch (p_tile_comp->tileType)
				{
				case TileTypes::GAME_FIELD:
					mpBuffer[index].color = PACK(p_color_comp->red, p_color_comp->green, p_color_comp->blue, 0);
					break;
				case TileTypes::WATER:
					mpBuffer[index].color = PACK(0, 0, 200 + color_offset, 0);
					break;
				case TileTypes::UNDEFINED:
					mpBuffer[index].color = PACK(0, 0, 0, 255);
					break;
				default:
					mpBuffer[index].color = PACK(255, 255, 255, 255);
					break;
				}

				index++;
			}
		}

		void TileRenderSystem::SetBegin(void* pBufferStart)
		{
			mpBuffer = (InputLayout*)pBufferStart;
		}
#pragma endregion TileRenderSystem

#pragma region SceneObjectRenderSystem
		SceneObjectRenderSystem::SceneObjectRenderSystem()
		{
			updateType = SystemUpdateType::MultiEntityUpdate;
			typeFilter.addRequirement(components::SceneObjectComponent::typeID);
			typeFilter.addRequirement(components::TransformComponent::typeID);
			typeFilter.addRequirement(components::ColorComponent::typeID);
		}

		SceneObjectRenderSystem::~SceneObjectRenderSystem()
		{

		}

		void SceneObjectRenderSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
		{

			uint32_t index = 0;
			for (FilteredEntity object : _entities.entities)
			{
				components::SceneObjectComponent* p_obj_comp = object.getComponent<components::SceneObjectComponent>();
				components::TransformComponent* p_transform_comp = object.getComponent<components::TransformComponent>();
				components::ColorComponent* p_color_comp = object.getComponent<components::ColorComponent>();

				mpBuffer[index].x = p_transform_comp->position.x;
				mpBuffer[index].y = p_transform_comp->position.y;
				mpBuffer[index].z = p_transform_comp->position.z;

				mpBuffer[index].color = PACK(p_color_comp->red, p_color_comp->green, p_color_comp->blue, 0);
				index++;
			}
		}

		void SceneObjectRenderSystem::SetBegin(void* pBufferStart)
		{
			mpBuffer = (InputLayout*)pBufferStart;
		}
#pragma endregion SceneObjectRenderSystem
	}
}