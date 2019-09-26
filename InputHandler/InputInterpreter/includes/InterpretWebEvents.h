#pragma once
#include "ecsEventIncludes.h"


namespace ecs
{
	namespace events
	{
		enum FSMStates
		{
			IDLE = 0,
			MOVE = -1,
			ERROR = -1
		};

		struct ChangeUserStateEvent : public ecs::ECSEvent<ChangeUserStateEvent>
		{
			int playerId = -1;
			FSMStates newState = FSMStates::ERROR;
		};
	}
}