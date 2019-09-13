#pragma once

#include <map>
#include "ecsGlobals.h"
#include "ecsComponent.h"
#include "ecsComponentPool.h"
#include "ecsTypeFilter.h"

namespace ecs
{
	class ECSComponentManager
	{
	public:
		ECSComponentManager();
		~ECSComponentManager();

		void initPoolType(TypeID _typeID, size_t _componentSize, size_t _count);

		BaseComponent* createComponent(BaseComponent& _initInfo);
		void removeComponent(TypeID _typeID, ID _id);

		BaseComponent* getComponent(TypeID _typeID, ID _id);
		ComponentIterator getComponentIterator(TypeID _typeID);


		void flagRemoval(TypeID _typeID, ID _componentID);
		void removeAllFlagged();

		size_t getComponentCountOfType(TypeID _typeID);
		size_t getTotalComponentCount();
		size_t getComponentTypeCount();

		TypeFilter getInitializedComponentTypes();

	private:
		using PoolPair = std::pair<TypeID, ECSComponentPool>;
		std::map<TypeID, ECSComponentPool> componentPools;
		std::map<TypeID, std::vector<ID>> toRemove;
	};
}