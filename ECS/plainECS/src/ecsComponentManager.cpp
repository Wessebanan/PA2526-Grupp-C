#include "ecsComponentManager.h"

using namespace ecs;

ECSComponentManager::ECSComponentManager()
{
	//
}

ECSComponentManager::~ECSComponentManager()
{
	//
}

void ECSComponentManager::initPoolType(TypeID _typeID, size_t _componentSize, size_t _count)
{
	componentPools[_typeID].initialize(_count, _componentSize);
}

BaseComponent* ECSComponentManager::createComponent(BaseComponent& _initInfo)
{
	TypeID typeID = _initInfo.getTypeID();

	// Check if memory pool for component type exists
	if (componentPools.count(typeID) == 0 || !componentPools[typeID].initialized)
	{
		componentPools[typeID].initialize(100, _initInfo.getSize());
	}
	
	return componentPools[typeID].add(_initInfo);
}

void ECSComponentManager::removeComponent(TypeID _typeID, ID _id)
{
	// Check if memory pool for component type exists
	if (componentPools.count(_typeID) == 0)
	{
		return;
	}

	componentPools[_typeID].remove(_id);
}

BaseComponent* ECSComponentManager::getComponent(TypeID _typeID, ID _id)
{
	// Check if memory pool for component type exists
	if (componentPools.count(_typeID) == 0)
	{
		return nullptr;
	}

	return componentPools[_typeID].getComponent(_id);
}

ComponentIterator ecs::ECSComponentManager::getComponentIterator(TypeID _typeID)
{
	return componentPools[_typeID].getIterator();
}

size_t ECSComponentManager::getPoolSize(TypeID _typeID)
{
	// If no pool of type exists, return 0.
	return componentPools.count(_typeID) ? componentPools[_typeID].getAllocations() : 0;
}

void ECSComponentManager::flagRemoval(TypeID _typeID, ID _componentID)
{
	componentPools[_typeID].flagRemoval(_componentID);
}

void ECSComponentManager::removeAllFlagged()
{
	std::map<TypeID, ECSComponentPool>::iterator it;

	for (it = componentPools.begin(); it != componentPools.end(); it++)
	{
		it->second.removeAllFlagged();
	}
}
