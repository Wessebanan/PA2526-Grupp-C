#include "ecsEntityManager.h"

using namespace ecs;

ECSEntityManager::ECSEntityManager() : idGenerator(1)
{
	//
}

ECSEntityManager::~ECSEntityManager()
{
	for (EntityPair pair : entities)
	{
		delete pair.second;
	}
	entities.clear();
}

Entity* ECSEntityManager::createEntity()
{
	return createEntityInternal();
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

#include <iostream>
void ECSEntityManager::flagRemoval(ID _entityID)
{
	if (!getEntity(_entityID))
	{
		std::cout << "ERROR: Non-existent entity is flagged for removal.\n";
		system("pause");

		throw "ERROR: Non-existent entity is flagged for removal.";
	}

	toRemove.push_back(_entityID);
}

void ECSEntityManager::removeAllFlagged()
{
	while (toRemove.size())
	{
		delete entities[toRemove.back()];	// Delete entity pointer, using to remove list
		entities.erase(toRemove.back());	// Erase entity entry in entity list, using to remove list
		toRemove.pop_back();				// Remove used entry in the to remove list
	}
}

size_t ECSEntityManager::getEntityCount()
{
	return entities.size();
}

unsigned int ecs::ECSEntityManager::getCurrentRemoveFlagCount()
{
	return (unsigned int)toRemove.size();
}

Entity* ECSEntityManager::createEntityInternal()
{

	// idGenerator is working like a counter, making sure that
	// all IDs it generate is unique.
	ID newID = idGenerator.generateID();
	Entity* entity = new Entity;
	entity->id = newID;
	return entities[newID] = entity;
}
