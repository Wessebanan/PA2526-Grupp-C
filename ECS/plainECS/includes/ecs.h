#pragma once

#include <map>
#include <vector>
#include "ecsComponent.h"
#include "ecsSystem.h"
#include "ecsGlobals.h"
#include "ecsComponentManager.h"
#include "ecsEntityManager.h"
#include "ecsEventManager.h"
#include "ecsUser.h"
#include "ecsEventListener.h"
#include "ecsEventIterator.h"
#include "ecsTypeFilter.h"
#include "ecsEvent.h"

#define LAYER_COUNT 3

namespace ecs
{
	struct CompTypeMemDesc
	{	
		TypeID typeID;
		size_t componentSize;
		size_t memoryCount;
	};

	struct ECSDesc
	{
		CompTypeMemDesc* compTypeMemDescs;
		size_t compTypeCount;
		size_t systemLayerCount;
	};

	class EntityComponentSystem : public ECSUserListener, public ECSEventListenerListener
	{
	public:
		EntityComponentSystem();
		~EntityComponentSystem();

		bool initialize(ECSDesc &_desc);

		ID createEntity(BaseComponent& _comp);
		ID createEntity(BaseComponent& _compA, BaseComponent& _compB);
		ID createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC);
		ID createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC, BaseComponent& _compD);
		ID createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC, BaseComponent& _compD, BaseComponent& _compE);
		ID createEntity(ComponentList _components);
		void removeEntity(ID _entityID);

		ID createComponent(ID _entityID, BaseComponent& _component);
		void removeComponent(ID _entityID, TypeID _componentTypeID);

		template <typename T>
		T* createSystem(unsigned int layer = 1);

		template <typename T>
		void removeSystem();

		void update(float _delta);

	private:
		ECSEventManager eventMgr;
		ECSEntityManager entityMgr;
		ECSComponentManager componentMgr;

		size_t layerCount;
		using SystemList = std::vector<BaseSystem*>;
		SystemList *systemLayers;
		std::map<TypeID, unsigned int> typeIDLayerMap;

		std::map<TypeID, std::vector<BaseSystem*>> eventListeners;

		// ECSUserListener virtual functions
		Entity* onGetEntity(ID _entityID) override;
		BaseComponent* onGetComponent(TypeID _typeID, ID _id) override;
		ID onCreateEntity(ComponentList _components) override;
		ID onCreateComponent(ID _entityID, BaseComponent& _componentInfo) override;
		void onCreateEvent(BaseEvent& _event) override;
		void onRemoveEntity(ID _entityID) override;
		void onRemoveComponent(ID _entityID, TypeID _componentTypeID) override;

		// ECSListenerListener virtual functions
		virtual void onAddSubscription(TypeID _eventTypeID, ECSEventListener* _listener);
		virtual void onRemoveSubscription(TypeID _eventTypeID, ECSEventListener* _listener);

		Entity* createEntityInternal(ComponentList _components);
		ID createComponentInternal(ID _entityID, BaseComponent& _componentInfo);
		void createEventInternal(BaseEvent& _event);
		void removeEntityInternal(ID _entityID);
		void removeComponentInternal(ID _entityID, TypeID _componentTypeID);
		void fillEntityIteratorInternal(TypeFilter& _componentFilter, EntityIterator& _iterator);
		void fillEventIteratorInternal(TypeFilter& _eventFilter, EventTypeIterator& _iterator);
		void notifyEventListeners(TypeID _eventType, BaseEvent *_pEvent);
	};

	/*
	*	Templated functions (has to be in header)
	*/

	template<typename T>
	inline T* EntityComponentSystem::createSystem(unsigned int layer)
	{
		// Check if systemLayers been initialized
		if (!systemLayers)
		{
			systemLayers = new SystemList[10];
		}

		if (typeIDLayerMap.count(T::typeID) != 0)
		{
			return nullptr;
		}
		T* newSystem = new T;
		(dynamic_cast<ECSUser*>(newSystem))->ecsUserHandler = this;


		//(dynamic_cast<ECSEventListener*>(newSystem))->eventListenerHandler = this;
		//UpdateSystemHandle* pUpdateSys = dynamic_cast<UpdateSystemHandle*>(newSystem);
		TypeFilter eventSubscriptions = newSystem->eventListeningFilter;
		if (!eventSubscriptions.requirements.empty())
		{
			for (TypeID typeID : eventSubscriptions.requirements)
			{
				eventListeners[typeID].push_back(newSystem);
			}
		}

		ECSEventListener* test = dynamic_cast<ECSEventListener*>(newSystem);

		typeIDLayerMap[T::typeID] = layer;
		systemLayers[layer].push_back(newSystem);
		return newSystem;
	}

	template<typename T>
	inline void EntityComponentSystem::removeSystem()
	{
		if (typeIDLayerMap.count(T::typeID) == 0)
		{
			return;
		}

		SystemList& layer = systemLayers[typeIDLayerMap[T::typeID]];
		for (size_t i = 0; i < layer.size(); i++)
		{
			if (layer[i]->getTypeID() == T::typeID)
			{
				layer.erase(layer.begin() + i);
				break;
			}
		}

		typeIDLayerMap.erase(T::typeID);
	}
}