#pragma once

#include "ecsEvent.h"
#include "ecsTypeFilter.h"

namespace ecs
{
	struct ECSEventListener;

	struct ECSEventListenerListener
	{
		virtual void onAddSubscription(TypeID _eventTypeID, ECSEventListener* _listener) = 0;
		virtual void onRemoveSubscription(TypeID _eventTypeID, ECSEventListener* _listener) = 0;
	};

	struct ECSEventListener
	{
		virtual void onEvent(TypeID _eventType, BaseEvent* _event) {};

	protected:
		void subscribeEventCreation(TypeID _eventType);
		void unsubscribeEventCreation(TypeID _eventType);
		TypeFilter eventTypes;

	private:
		void notifyHandler();
		TypeFilter newSubscriptions;
		TypeFilter newUnsubscriptions;
		ECSEventListenerListener* eventListenerHandler = nullptr;
		friend class EntityComponentSystem;
	};
}