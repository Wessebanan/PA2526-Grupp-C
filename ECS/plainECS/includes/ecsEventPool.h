#pragma once

#include <vector>
#include "ecsEvent.h"
#include "ecsEventIterator.h"

namespace ecs
{
	class ECSEventPool
	{
	public:
		ECSEventPool();
		~ECSEventPool();

		BaseEvent* create(BaseEvent& _event);
		std::vector<BaseEvent*> getIterator();

		void clear();

	private:

		std::vector<BaseEvent*> events;
	};
}