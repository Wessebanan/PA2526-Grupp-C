#include "ecsEntity.h"

using namespace ecs;

ID Entity::getID()
{
	return id;
}

ID Entity::getComponentID(TypeID _typeID)
{
	// Check if component of type exists
	if (componentIDs.count(_typeID) == 0)
	{
		return 0;
	}

	return componentIDs[_typeID];
}

bool Entity::hasComponentOfType(TypeID _typeID)
{
	if (componentIDs.count(_typeID) == 0)
	{
		return false;
	}
	return true;
}