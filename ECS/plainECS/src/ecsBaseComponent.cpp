#include "ecsBaseComponent.h"

using namespace ecs;

// Make id counters start at 1, since 0=error
//IDGenerator<TypeID> BaseComponent::typeIDGenerator(1);
//IDGenerator<ID> BaseComponent::uniqueIDGenerator(1);

TypeID BaseComponent::typeIDCounter(1);
TypeID BaseComponent::uniqueIDCounter(1);
