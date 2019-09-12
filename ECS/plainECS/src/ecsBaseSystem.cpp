#include "ecsBaseSystem.h"

using namespace ecs;

// Make id counters start at 1, since 0=error
TypeID BaseSystem::typeIDCounter(1);
//IDGenerator<TypeID> BaseSystem::typeIDGenerator(1);
