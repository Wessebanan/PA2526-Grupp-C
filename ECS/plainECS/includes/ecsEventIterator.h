#pragma once

#include <map>
#include <vector>
#include "ecsEvent.h"
#include "ecsGlobals.h"

namespace ecs
{
	//class EventIterator
	//{
	//	EventIterator() {}
	//	EventIterator(std::vector<BaseEvent*>& _events);
	//	~EventIterator() {}
	//	std::vector<BaseEvent*> events;

	//};

	struct EventTypeIterator
	{
		using TypeList = std::map<TypeID, std::vector<BaseEvent*>>;
		using TypePair = std::pair<TypeID, std::vector<BaseEvent*>>;
		TypeList eventTypes;

		EventTypeIterator() = default;
		EventTypeIterator(const EventTypeIterator& _other)
		{
			for (TypePair list : _other.eventTypes)
			{
				//for (size_t i = 0; i < list.second.size(); i++)
				//{
				//	eventTypes[list.first][i] = list.second[i];
				//}
				eventTypes[list.first] = list.second;
			}
		}
	};

//	class EventIterator
//	{
//	public:
//		EventIterator();
//		~EventIterator();
//
//		std::vector<BaseEvent*>* events;
//	private:
//		EventIterator(std::vector<BaseEvent*>* _rEventList);
//		friend class ECSEventPool;
//	};
//
//	struct EventIteratorContainer
//	{
//		std::map<TypeID, EventIterator> iteratorTypes;
//	};
}