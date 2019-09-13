#include "ecsEventPool.h"

using namespace ecs;

ECSEventPool::ECSEventPool()
{
	//
}

ecs::ECSEventPool::~ECSEventPool()
{
	clear();
}

BaseEvent* ECSEventPool::create(BaseEvent& _event)
{
	// Retrieve the event's create function
	EventCreateFunction cf = _event.getCreateFunction();

	BaseEvent* newEvent = cf(&_event);
	events.push_back(newEvent);
	return newEvent;
}

std::vector<BaseEvent*> ECSEventPool::getIterator()
{
	return events;
}

void ECSEventPool::clear()
{
	for (BaseEvent* e : events)
	{
		// Retrieve the event's free function
		EventFreeFunction ff = e->getFreeFunction();
		ff(e);
	}
	events.clear();
}
//
//ecs::EventIterator::EventIterator()
//{
//	events = nullptr;
//}
//
//EventIterator::~EventIterator()
//{
//	//
//}
//
//EventIterator::EventIterator(std::vector<BaseEvent*>* _rEventList) : events(_rEventList)
//{
//	currentIndex = 0;
//}