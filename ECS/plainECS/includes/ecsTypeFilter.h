#pragma once

#include <vector>
#include "ecsGlobals.h"

namespace ecs
{
	class TypeFilter
	{
	public:
		TypeFilter();
		~TypeFilter();

		void addRequirement(TypeID _typeID);
		void removeRequirement(TypeID _typeID);

		std::vector<TypeID> getRequirements();

	private:
		std::vector<TypeID> requirements;
		friend class EntityComponentSystem;
		friend struct ECSEventListener;
	};
}