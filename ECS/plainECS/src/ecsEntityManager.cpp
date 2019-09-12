#include "ecsEntityManager.h"

using namespace ecs;

ECSEntityManager::ECSEntityManager() : idGenerator(1)
{
	//
}

ECSEntityManager::~ECSEntityManager()
{
	//
}

Entity* ECSEntityManager::createEntity()
{
	return createEntityInternal();
}

void ecs::ECSEntityManager::removeEntity(ID _entityID)
{
	if (entities.count(_entityID) == 0)
	{
		return;
	}
	delete entities[_entityID];
	entities.erase(_entityID);
}

Entity* ECSEntityManager::getEntity(ID _entityID)
{
	// Check if entity exist
	if (entities.count(_entityID) == 0)
	{
		return nullptr;
	}
	return entities[_entityID];
}

void ECSEntityManager::flagRemoval(ID _entityID)
{
	toRemove.push_back(_entityID);
}

void ECSEntityManager::removeAllFlagged()
{
	while (toRemove.size())
	{
		delete entities[toRemove.back()];
		entities.erase(toRemove.back());
		toRemove.pop_back();
	}
}

Entity* ECSEntityManager::createEntityInternal()
{
	ID newID = idGenerator.generateID();
	Entity* entity = new Entity;
	entity->id = newID;
	return entities[newID] = entity;
}
