#pragma once

#include "../../ECS/plainECS/includes/ecsSystemIncludes.h"
#include "GraphicsComponents.h"

#include "DeviceInterface.h"
#include <map>

namespace ecs
{
	namespace systems
	{
		constexpr unsigned int compCount = 4000;

		struct MeshRenderSystemData
		{
			UINT m_maximumMeshes;
			std::map<UINT, UINT> m_meshCount;
			DirectX::XMFLOAT4X4 m_matrices[compCount];
		};

		class MeshRenderSystem : public ECSSystem<MeshRenderSystem>
		{
		public:
			MeshRenderSystem()
			{
				updateType = SystemUpdateType::MultiEntityUpdate;

				typeFilter.addRequirement(components::WorldComponent::typeID);
				typeFilter.addRequirement(components::MeshComponent::typeID);

				subscribeEventCreation(events::CreateComponentEvent::typeID);
				subscribeEventCreation(events::RemoveComponentEvent::typeID);

				m_pData = NULL;
			}

			~MeshRenderSystem()
			{
			
			}

			void onEvent(TypeID _eventType, BaseEvent* _event) override
			{
				if (_eventType == events::CreateComponentEvent::typeID)
				{
					CreateComponentEvent* e = (CreateComponentEvent*)_event;
					
					if (e->componentTypeID == MeshComponent::typeID)
					{
						MeshComponent* mc = (MeshComponent*)getComponent(MeshComponent::typeID, e->componentID);
						
						if (!m_pData->m_meshCount.count(mc->MeshLocation.ID))
						{
							m_pData->m_meshCount[mc->MeshLocation.ID] = 0;
						}

						m_pData->m_meshCount[mc->MeshLocation.ID]++;

						m_pData->m_maximumMeshes++;
					}
				}
				else if (_eventType == events::RemoveComponentEvent::typeID)
				{
					RemoveComponentEvent* e = (RemoveComponentEvent*)_event;

					if (e->componentTypeID == MeshComponent::typeID)
					{
						MeshComponent* mc = (MeshComponent*)getComponent(MeshComponent::typeID, e->componentID);

						if (!m_pData->m_meshCount[mc->MeshLocation.ID] == 1)
						{
							m_pData->m_meshCount.erase(mc->MeshLocation.ID);
						}
						else
						{
							m_pData->m_meshCount[mc->MeshLocation.ID]--;
						}

						m_pData->m_maximumMeshes--;
					}
				}


			}

			void updateMultipleEntities(EntityIterator& _entities, float _delta) override
			{
				std::map<UINT, UINT> offsets;

				components::WorldComponent* wc = NULL;
				components::MeshComponent* mc = NULL;
				for (FilteredEntity entity : _entities.entities)
				{
					wc = entity.getComponent<components::WorldComponent>();
					mc = entity.getComponent<components::MeshComponent>();

					if (!offsets.count(mc->MeshLocation.ID))
					{
						offsets[mc->MeshLocation.ID] = 0;
					}

					if (mc->MeshLocation.ID == 1)
					{
						int x = 0;
					}

					UINT offset = offsets[mc->MeshLocation.ID]++;
					UINT location = 0;
					for (int i = mc->MeshLocation.ID - 1; i >= 0; i--)
					{
						UINT count = (UINT)m_pData->m_meshCount[i];
						location += count;
					}

					UINT index = location + offset;
					if (index < compCount)
					{
						m_pData->m_matrices[index] = wc->WorldMatrix;
					}
				}
			}

			//void insert(ID id, )

			MeshRenderSystemData* m_pData;
		};
	}
}