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

	struct ECSUser
	{
	protected:
		Entity* getEntity(ID _entityID);
		EntityIterator getEntitiesByFilter(TypeFilter _componentFilter);
		EntityIterator getEntitiesWithComponent(TypeID _typeID);
		ComponentIterator getComponentsOfType(TypeID _typeID);
		BaseComponent* GetComponent(TypeID _typeID, ID _id);
		Entity* createEntity(BaseComponent& _comp);
		Entity* createEntity(BaseComponent& _compA, BaseComponent& _compB);
		Entity* createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC);
		Entity* createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC, BaseComponent& _compD);
		Entity* createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC, BaseComponent& _compD, BaseComponent& _compE);
		Entity* createEntity(ComponentList _components);

		template <typename T> EntityIterator getEntitiesWithComponent();
		template <typename T> ComponentIterator getComponentsOfType();
		template <typename T> T* createComponent(ID _entityID, T& _componentInfo);

		void createEvent(BaseEvent& _event);
		void removeEntity(ID _entityID);
		void removeComponent(ID _entityID, TypeID _componentTypeID);

		TypeFilter componentFilter;
		TypeFilter eventFilter;

	private:
		ECSUserListener* ecsUserHandler = nullptr;
		friend class EntityComponentSystem;
	};



	/*
	*	Templated functions (has to be in header)
	*/

	template <typename T>
	EntityIterator ECSUser::getEntitiesWithComponent()
	{
		TypeFilter filter;
		filter.addRequirement(T::typeID);
		return ecsUserHandler->getEntitiesByFilter(filter);
	}

	template <typename T>
	ComponentIterator ECSUser::getComponentsOfType()
	{
		return ecsUserHandler->getEntitiesByFilter(T::typeID);
	}

	template <typename T>
	T* ECSUser::createComponent(ID _entityID, T& _componentInfo)
	{
		return (T*)ecsUserHandler->onCreateComponent(_entityID, _componentInfo);
	}
}