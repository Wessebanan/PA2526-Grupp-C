#include "ecsEventListener.h"

using namespace ecs;

void ECSEventListener::subscribeEventCreation(TypeID _eventType)
{
	eventListenerHandler->onAddSubscription(_eventType, this);
}

void ECSEventListener::unsubscribeEventCreation(TypeID _eventType)
{
	eventListenerHandler->onRemoveSubscription(_eventType, this);
}