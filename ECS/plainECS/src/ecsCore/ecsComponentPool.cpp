#include "ecsComponentPool.h"

using namespace ecs;

ECSComponentPool::ECSComponentPool()
{
	componentCap = 0;
	componentSize = 0;
	memorySize = 0;
	memory = nullptr;
}

ECSComponentPool::ECSComponentPool(size_t _startCap, size_t _componentSize)
{
	initialize(_startCap, _componentSize);
}

ECSComponentPool::~ECSComponentPool()
{
	free(memory);
}

BaseComponent* ECSComponentPool::add(BaseComponent& _component)
{
	void* p = allocator.allocate();

	if (p == nullptr)
	{
		// TODO: Add another allocator
		return 0;
	}

	ComponentCreateFunction create = _component.getCreateFunction();

	ID newID = create(p, &_component);
	if (newID == 0)
	{
		return nullptr;
	}

	lookUpList[_component.getID()] = (BaseComponent*)p;
	return (BaseComponent*)p;
}

void ECSComponentPool::remove(ID _id)
{
	// Check if id exists
	if (lookUpList.count(_id) == 0)
	{
		return;
	}

	allocator.free(lookUpList[_id]);
	lookUpList.erase(_id);
}

size_t ECSComponentPool::getAllocations()
{
	return allocator.getAllocations();
}

BaseComponent* ECSComponentPool::getComponent(ID _id)
{
	// Check if id exists
	if (lookUpList.count(_id) == 0)
	{
		return nullptr;
	}

	return lookUpList[_id];
}

ComponentIterator ECSComponentPool::getIterator()
{
	return ComponentIterator(componentSize, memorySize, memory);
}

void ECSComponentPool::initialize(size_t _startCap, size_t _componentSize)
{
	componentCap = _startCap;
	componentSize = _componentSize;
	memorySize = componentCap * componentSize;
	memory = (void*)malloc(memorySize);
	std::memset(memory, 0, memorySize);
	allocator.initialize(memory, memorySize, componentSize);
	initialized = true;
}

void ecs::ECSComponentPool::flagRemoval(ID _componentID)
{
	lookUpList[_componentID]->alive = false;
	toRemove.push_back(_componentID);
}

void ecs::ECSComponentPool::removeAllFlagged()
{
	while (toRemove.size())
	{
		ID id = toRemove.back();
		if (lookUpList.count(id))
		{
			void* p = lookUpList[id];
			lookUpList.erase(id);
			allocator.free(p);
			toRemove.pop_back();
		}
	}
}

ComponentIterator::ComponentIterator(size_t _objectSize, size_t _totalSize, void* _memory)
{
	current = first = _memory;
	totalSize = _totalSize;
	objectSize = _objectSize;
	iterationSize = 0;
}

ComponentIterator::~ComponentIterator()
{
	//
}

BaseComponent* ComponentIterator::next()
{
	// Iterate in memory until next alive component is found (!alive => deleted),
	// or end of memory is reached.
	BaseComponent* pComponent;
	do
	{
		if (iterationSize >= totalSize)
		{
			return nullptr;
		}

		pComponent = (BaseComponent*)current;
		current = (void*)((char*)current + objectSize);
		iterationSize += objectSize;
	} while (!pComponent->alive);

	return pComponent;
}