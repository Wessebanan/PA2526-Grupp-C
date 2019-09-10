#pragma once

#include "ecsEvent.h"
#include "ecsEntity.h"
#include "ecsTypeFilter.h"
#include "ecsComponent.h"

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
		virtual ID onCreateEntity(ComponentList _components) = 0;
		virtual ID onCreateComponent(ID _entityID, BaseComponent& _componentInfo) = 0;
		virtual void onCreateEvent(BaseEvent& _event) = 0;
		virtual void onRemoveEntity(ID _entityID) = 0;
		virtual void onRemoveComponent(ID _entityID, TypeID _componentTypeID) = 0;
	};

	struct ECSUser
	{
	protected:
		Entity* getEntity(ID _entityID);
		BaseComponent* GetComponent(TypeID _typeID, ID _id);
		ID createEntity(BaseComponent& _comp);
		ID createEntity(BaseComponent& _compA, BaseComponent& _compB);
		ID createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC);
		ID createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC, BaseComponent& _compD);
		ID createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC, BaseComponent& _compD, BaseComponent& _compE);
		ID createEntity(ComponentList _components);
		ID createComponent(ID _entityID, BaseComponent& _componentInfo);
		void createEvent(BaseEvent& _event);
		void removeEntity(ID _entityID);
		void removeComponent(ID _entityID, TypeID _componentTypeID);

		TypeFilter componentFilter;
		TypeFilter eventFilter;

	private:
		ECSUserListener* ecsUserHandler = nullptr;
		friend class EntityComponentSystem;
	};
}