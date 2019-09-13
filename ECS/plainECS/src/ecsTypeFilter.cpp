#include "ecsTypeFilter.h"

using namespace ecs;

TypeFilter::TypeFilter()
{
	//
}

TypeFilter::~TypeFilter()
{
	//
}

void TypeFilter::addRequirement(TypeID _typeID)
{
	// Check if requirement already exists
	for (TypeID t : requirements)
	{
		if (t == _typeID)
		{
			return;
		}
	}
	requirements.push_back(_typeID);
}

void TypeFilter::removeRequirement(TypeID _typeID)
{
	for (size_t i = 0; i < requirements.size(); i++)
	{
		if (requirements[i] == _typeID)
		{
			requirements.erase(requirements.begin() + i);
			return;
		}
	}
}

std::vector<TypeID> ecs::TypeFilter::getRequirements()
{
	return requirements;
}
