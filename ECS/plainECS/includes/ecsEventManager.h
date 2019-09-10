#pragma once

#include <vector>
#include "ecsEvent.h"
#include "ecsEventPool.h"
#include "ecsEventListener.h"

namespace ecs
{
	class ECSEventManager
	{
	public:

		ECSEventManager();
		~ECSEventManager();

		BaseEvent* createEvent(BaseEvent& _event);
		void addEventSubscriber(TypeID _eventTypeID, ECSEventListener* _listener);
		void removeEventSubscriber(TypeID _eventTypeID, ECSEventListener* _listener);

		std::vector<BaseEvent*> getEventIterator(TypeID _eventTypeID);

		void clearAllEvents();

	private:
		std::map<TypeID, ECSEventPool> eventPools;
		std::map<TypeID, std::vector<ECSEventListener*>> instantListeners;

		void notifyInstantListenersInternal(BaseEvent* _event);
	};
}