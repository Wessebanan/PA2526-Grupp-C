#include "ecsEventListener.h"

using namespace ecs;

void ECSEventListener::subscribeEventCreation(TypeID _eventType)
{
	//eventListenerHandler->onAddSubscription(_eventType, this);
	eventTypes.addRequirement(_eventType);
	newSubscriptions.addRequirement(_eventType);

	if (eventListenerHandler)
	{
		notifyHandler();
	}
}

void ECSEventListener::unsubscribeEventCreation(TypeID _eventType)
{
	//eventListenerHandler->onRemoveSubscription(_eventType, this);
	eventTypes.removeRequirement(_eventType);
	newUnsubscriptions.addRequirement(_eventType);

	if (eventListenerHandler)
	{
		notifyHandler();
	}
}

void ecs::ECSEventListener::notifyHandler()
{
	for (TypeID typeID : newSubscriptions.requirements)
	{
		eventListenerHandler->onAddSubscription(typeID, this);
	}

	for (TypeID typeID : newUnsubscriptions.requirements)
	{
		eventListenerHandler->onRemoveSubscription(typeID, this);
	}
}
