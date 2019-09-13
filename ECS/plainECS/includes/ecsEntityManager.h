#pragma once

#include <map>
#include "ecsEntity.h"
#include "ecsGlobals.h"
#include "ecsIDGenerator.h"

namespace ecs
{
	class ECSEntityManager
	{
	public:
		ECSEntityManager();
		~ECSEntityManager();

		Entity* createEntity();
		void removeEntity(ID _entityID);

		Entity* getEntity(ID _entityID);

		void flagRemoval(ID _entityID);
		void removeAllFlagged();

		size_t getEntityCount();

	private:
		IDGenerator<ID> idGenerator;
		std::map<ID, Entity*> entities;
		std::vector<ID> toRemove;

		Entity* createEntityInternal();
	};
}