#pragma once

#include "ecsEvent.h"
#include "ecsEntity.h"
#include "ecsTypeFilter.h"
#include "ecsComponent.h"
#include "ecsComponentIterator.h"

namespace ecs
{
	struct ComponentList
	{
		BaseComponent** initialInfo;
		size_t componentCount;
	};

	/*
		ECSUserListener observe an ECSUser and provide ECS resources on notifications.
	*/
	struct ECSUserListener
	{
		virtual Entity* onGetEntity(ID _entityID) = 0;
		virtual BaseComponent* onGetComponent(TypeID _typeID, ID _id) = 0;
		virtual Entity* onCreateEntity(ComponentList _components) = 0;
		virtual BaseComponent* onCreateComponent(ID _entityID, BaseComponent& _componentInfo) = 0;
		virtual EntityIterator getEntitiesByFilter(TypeFilter _componentFilter) = 0;
		virtual ComponentIterator getComponentsOfType(TypeID _typeID) = 0;
		virtual void onCreateEvent(BaseEvent& _event) = 0;
		virtual void onRemoveEntity(ID _entityID) = 0;
		virtual void onRemoveComponent(ID _entityID, TypeID _componentTypeID) = 0;
	};

	/*
		ECSUser provides functionality to create, remove and fetch information when working with ECS.
		Typically, custom system is a child of ECSUser.
		ECSUser is an 'observable' in the observer-pattern, where an ECSUserListener is the observer.
	*/
	struct ECSUser
	{
		// TODO: Move ECSUser functionality to BaseSystem.
	protected:

		// Returns a pointer to an entity with given ID.
		Entity* getEntity(ID _entityID);

		// Returns an iterator with entities that owns all of the required component types.
		EntityIterator getEntitiesByFilter(TypeFilter _componentFilter);

		// Returns an iterator with entities that owns a component of requested type.
		EntityIterator getEntitiesWithComponent(TypeID _typeID);

		// Returns an iterator with components of requested type.
		ComponentIterator getComponentsOfType(TypeID _typeID);

		// Returns a base pointer to a requested component.
		// In order to use the component, the returning pointer has to be casted manually.
		// If component type is known at compile time, a templated version of this function
		// can be used instead.
		BaseComponent* getComponent(TypeID _typeID, ID _id);

		// Returns a base pointer to a requested component, that is own from given entity.
		// In order to use the component, the returning pointer has to be casted manually.
		// If component type is known at compile time, a templated version of this function
		// can be used instead.
		BaseComponent* getComponentFromKnownEntity(TypeID _typeID, ID _entityID);

		// Creates both a new entity and a new component in ECS memory, and returns a pointer
		// to the entity.
		Entity* createEntity(BaseComponent& _comp);

		// Creates both a new entity and two new components in ECS memory, and returns a pointer
		// to the entity.
		Entity* createEntity(BaseComponent& _compA, BaseComponent& _compB);

		// Creates both a new entity and three new components in ECS memory, and returns a pointer
		// to the entity.
		Entity* createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC);

		// Creates both a new entity and four new components in ECS memory, and returns a pointer
		// to the entity.
		Entity* createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC, BaseComponent& _compD);

		// Creates both a new entity and five new components in ECS memory, and returns a pointer
		// to the entity.
		Entity* createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC, BaseComponent& _compD, BaseComponent& _compE);

		// Creates both a new entity and multiple new components in ECS memory, and returns a pointer
		// to the entity.
		Entity* createEntity(ComponentList _components);

		/*
			Templated functions
		*/

		// Returns an iterator with entities that owns a component of requested type.
		template <typename T> EntityIterator getEntitiesWithComponent();

		// Returns an iterator with components of requested type.
		template <typename T> ComponentIterator getComponentsOfType();

		// Creates a new component in ECS memory that belongs to given entity, initializing it with
		// the given component data.
		// Returns a type casted pointer to the created component.
		template <typename T> T* createComponent(ID _entityID, T& _componentInfo);

		// Returns a type casted pointer to a requested component.
		template <typename T> T* getComponent(ID _componentID);

		// Returns a type casted pointer to a requested component that belongs to given entity.
		template <typename T> T* getComponentFromKnownEntity(ID _entityID);

		// Creates a new ECS event that is pushed to event queue.
		// Notifies all event listeners listening on that event type.
		void createEvent(BaseEvent& _event);

		void removeEntity(ID _entityID);
		void removeComponent(ID _entityID, TypeID _componentTypeID);

	private:

		ECSUserListener* ecsUserHandler = nullptr;
		friend class EntityComponentSystem;
	};



	/*
		Templated functions (has to be in header)
		All ECSUser methods forward resource requests to its handler.
	*/

	template <typename T>
	inline EntityIterator ECSUser::getEntitiesWithComponent()
	{
		TypeFilter filter;
		filter.addRequirement(T::typeID);
		return ecsUserHandler->getEntitiesByFilter(filter);
	}

	template <typename T>
	inline ComponentIterator ECSUser::getComponentsOfType()
	{
		return ecsUserHandler->getComponentsOfType(T::typeID);
	}

	template <typename T>
	inline T* ECSUser::createComponent(ID _entityID, T& _componentInfo)
	{
		return (T*)ecsUserHandler->onCreateComponent(_entityID, _componentInfo);
	}

	template<typename T>
	inline T* ECSUser::getComponent(ID _componentID)
	{
		return ecsUserHandler->onGetComponent(T::typeID, _componentID);
	}

	template<typename T>
	inline T* ECSUser::getComponentFromKnownEntity(ID _entityID)
	{
		Entity* e = ecsUserHandler->onGetEntity(_entityID);

		// Sanity check
		if (!e || !e->hasComponentOfType(T::typeID))
		{
			return nullptr;
		}

		return ecsUserHandler->onGetComponent(T::typeID, e->getComponentID(T::typeID));
	}
}