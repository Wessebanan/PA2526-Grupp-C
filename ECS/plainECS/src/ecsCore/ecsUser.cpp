#include "ecsUser.h"

using namespace ecs;

Entity* ECSUser::getEntity(ID _entityID)
{
	return ecsUserHandler->onGetEntity(_entityID);
}

BaseComponent* ECSUser::GetComponent(TypeID _typeID, ID _id)
{
	return ecsUserHandler->onGetComponent(_typeID, _id);
}

ID ECSUser::createEntity(BaseComponent& _comp)
{
	BaseComponent* components[] = { &_comp };

	ComponentList list;
	list.initialInfo = components;
	list.componentCount = 1;

	return ecsUserHandler->onCreateEntity(list);
}

ID ECSUser::createEntity(BaseComponent & _compA, BaseComponent & _compB)
{
	BaseComponent* components[] = { &_compA, &_compB };

	ComponentList list;
	list.initialInfo = components;
	list.componentCount = 2;

	return ecsUserHandler->onCreateEntity(list);
}

ID ECSUser::createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC)
{
	BaseComponent* components[] = { &_compA, &_compB, &_compC };

	ComponentList list;
	list.initialInfo = components;
	list.componentCount = 3;

	return ecsUserHandler->onCreateEntity(list);
}

ID ECSUser::createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC, BaseComponent& _compD)
{
	BaseComponent* components[] = { &_compA, &_compB, &_compC, &_compD };

	ComponentList list;
	list.initialInfo = components;
	list.componentCount = 4;

	return ecsUserHandler->onCreateEntity(list);
}

ID ECSUser::createEntity(BaseComponent& _compA, BaseComponent& _compB, BaseComponent& _compC, BaseComponent& _compD, BaseComponent& _compE)
{
	BaseComponent* components[] = { &_compA, &_compB, &_compC, &_compD, &_compE };

	ComponentList list;
	list.initialInfo = components;
	list.componentCount = 5;

	return ecsUserHandler->onCreateEntity(list);
}

ID ECSUser::createEntity(ComponentList _components)
{
	return ecsUserHandler->onCreateEntity(_components);
}

ID ECSUser::createComponent(ID _entityID, BaseComponent& _componentInfo)
{
	return ecsUserHandler->onCreateComponent(_entityID, _componentInfo);
}

void ECSUser::createEvent(BaseEvent& _event)
{
	ecsUserHandler->onCreateEvent(_event);
}

void ECSUser::removeEntity(ID _entityID)
{
	ecsUserHandler->onRemoveEntity(_entityID);
}

void ECSUser::removeComponent(ID _entityID, TypeID _componentTypeID)
{
	ecsUserHandler->onRemoveComponent(_entityID, _componentTypeID);
}
