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

#define DEFAULT_LAYER_COUNT 10

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

		Entity* createEntity(BaseComponent& _comp);
		Entity* createEntity(BaseComponent& _compA, BaseComponent& _compB);
		Entity* createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC);
		Entity* createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC, BaseComponent& _compD);
		Entity* createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC, BaseComponent& _compD, BaseComponent& _compE);
		Entity* createEntity(ComponentList _components);
		void removeEntity(ID _entityID);

		template <typename T>
		T* createComponent(ID _entityID, T& _component);
		void removeComponent(ID _entityID, TypeID _componentTypeID);

		template <typename T>
		T* createSystem(unsigned int layer = 1);

		template <typename T>
		void removeSystem();

		void update(float _delta);

		/*
		*	Getters
		*/

		Entity* getEntity(ID _id);
		BaseComponent* getComponent(TypeID _typeID, ID _id);
		BaseComponent* getComponentFromEntity(TypeID _typeID, ID _entityID);
		
		template <typename T> T* getComponent(ID _id);
		template <typename T> T* getComponentFromEntity(ID _entityID);

		size_t getSystemLayers();
		size_t getTotalSystemCount();
		size_t getTotalEntityCount();
		size_t getTotalComponentCount();
		size_t getComponentTypeCount();
		size_t getComponentCountOfType(TypeID _typeID);
		
		EntityIterator getFilteredEntityIterator(TypeFilter _componentFilter);
		ComponentIterator getAllComponentsOfType(TypeID _typeID);

		TypeFilter getInitializedComponentTypes();

	private:
		ECSEventManager eventMgr;
		ECSEntityManager entityMgr;
		ECSComponentManager componentMgr;

		size_t layerCount;
		//size_t systemsPerLayer;
		using SystemList = std::vector<BaseSystem*>;
		//using SystemList = BaseSystem**;
		SystemList *systemLayers;
		std::map<TypeID, unsigned int> typeIDLayerMap;

		// ECSUserListener virtual functions
		Entity* onGetEntity(ID _entityID) override;
		BaseComponent* onGetComponent(TypeID _typeID, ID _id) override;
		Entity* onCreateEntity(ComponentList _components) override;
		BaseComponent* onCreateComponent(ID _entityID, BaseComponent& _componentInfo) override;
		EntityIterator getEntitiesByFilter(TypeFilter _componentFilter) override;
		ComponentIterator getComponentsOfType(TypeID _typeID) override;
		void onCreateEvent(BaseEvent& _event) override;
		void onRemoveEntity(ID _entityID) override;
		void onRemoveComponent(ID _entityID, TypeID _componentTypeID) override;

		// ECSListenerListener virtual functions
		virtual void onAddSubscription(TypeID _eventTypeID, ECSEventListener* _listener);
		virtual void onRemoveSubscription(TypeID _eventTypeID, ECSEventListener* _listener);

		Entity* createEntityInternal(ComponentList _components);
		BaseComponent* createComponentInternal(ID _entityID, BaseComponent& _componentInfo);
		void createEventInternal(BaseEvent& _event);
		void removeEntityInternal(ID _entityID);
		void removeComponentInternal(ID _entityID, TypeID _componentTypeID);
		void fillEntityIteratorInternal(TypeFilter& _componentFilter, EntityIterator& _iterator);
		void fillEventIteratorInternal(TypeFilter& _eventFilter, EventTypeIterator& _iterator);
	};

	/*
	*	Templated functions (has to be in header)
	*/

	template<typename T>
	T* EntityComponentSystem::createComponent(ID _entityID, T& _component)
	{
		// Forwards to internal function.
		return (T*)createComponentInternal(_entityID, _component);
	}

	template<typename T>
	inline T* EntityComponentSystem::createSystem(unsigned int layer)
	{
		// Check if systemLayers been initialized
		if (!systemLayers)
		{
			layerCount = 10;
			systemLayers = new SystemList[10];
		}

		// Check if system is already added,
		// should only exist one system at most per type
		if (typeIDLayerMap.count(T::typeID) != 0)
		{
			return nullptr;
		}

		/*
		*	Create system, which calls its constructor.
		*	The system's constructor will set
		*		- UpdateType
		*		- EventSubscriptions (added to list, read later when ECS calls for it
		*		- Component/Event filter
		*/
		T* newSystem = new T;

		// Make ECS listen on ECSUser functionality. This makes ECS responsible for
		// handling entity creations, component removals etc.
		(dynamic_cast<ECSUser*>(newSystem))->ecsUserHandler = this;

		// Set ECS to handle all subscriptions and unsubscriptions on event creations.
		ECSEventListener* listenerCast = static_cast<ECSEventListener*>(newSystem);

		// Fetch all subscriptions. This must be done AFTER eventListenerHandler been set,
		// as it will call eventListerHandler's onAddSubscription-overrided function.
		listenerCast->eventListenerHandler = this;
		listenerCast->notifyHandler();

		// Set system in hashed list for easy access
		typeIDLayerMap[T::typeID] = layer;

		// Push back system in wanted layer for later update
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

	template <typename T> T* EntityComponentSystem::getComponent(ID _id)
	{
		return (T*)getComponent(T::typeID, _id);
	}

	template <typename T> T* EntityComponentSystem::getComponentFromEntity(ID _entityID)
	{
		return (T*)getComponentFromEntity(_entityID);
	}
}