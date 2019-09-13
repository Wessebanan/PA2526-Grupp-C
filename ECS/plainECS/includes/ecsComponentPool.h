#pragma once

#include <map>
#include <vector>
#include "ecsGlobals.h"
#include "poolAllocator.h"
#include "ecsBaseComponent.h"
#include "ecsComponentIterator.h"

namespace ecs
{
	class ComponentIterator;

	class ECSComponentPool
	{
	public:
		ECSComponentPool();
		ECSComponentPool(size_t _startCap, size_t _componentSize);
		~ECSComponentPool();

		BaseComponent* add(BaseComponent& _component);
		void remove(ID _id);

		size_t getAllocations();
		BaseComponent* getComponent(ID _id);
		ComponentIterator getIterator();

		void initialize(size_t _startCap, size_t _componentSize);

		void flagRemoval(ID _componentID);
		void removeAllFlagged();

		bool initialized = false;

	private:
		PoolAllocator allocator;
		std::map<ID, BaseComponent*> lookUpList;

		void* memory;
		size_t memorySize;
		size_t componentCap;
		size_t componentSize;

		std::vector<ID> toRemove;
	};
}