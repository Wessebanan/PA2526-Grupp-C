#pragma once

#include <map>
#include "ecsBaseEvent.h"
#include "ecsGlobals.h"

namespace ecs
{
	namespace events
	{
		struct CreateEntityEvent : public ECSEvent<CreateEntityEvent>
		{
			ID entityID;
		};

		struct RemoveEntityEvent : public ECSEvent<RemoveEntityEvent>
		{
			ID entityID;
		};

		struct CreateComponentEvent : public ECSEvent<CreateComponentEvent>
		{
			ID entityID;
			ID componentID;
			TypeID componentTypeID;
		};

		struct RemoveComponentEvent : public ECSEvent<RemoveComponentEvent>
		{
			ID entityID;
			ID componentID;
			TypeID componentTypeID;
		};




		struct MessageStringEvent : public ECSEvent<MessageStringEvent>
		{
			std::string messageString;
		};
	}
}