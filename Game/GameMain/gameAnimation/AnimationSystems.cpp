#include "AnimationSystems.h"
#include "../gameAI/AIComponents.h"
ecs::systems::SkeletonSystem::SkeletonSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(components::SkeletonComponent::typeID);
}

ecs::systems::SkeletonSystem::~SkeletonSystem()
{
}

void ecs::systems::SkeletonSystem::updateEntity(FilteredEntity& entity, float delta)
{
	components::SkeletonComponent* pSkeleton = entity.getComponent<components::SkeletonComponent>();

	if (getComponentFromKnownEntity<ecs::components::AttackStateComponent>(pSkeleton->getEntityID()))
	{
	pSkeleton->skeletonData.UpdateAnimation(delta, ModelLoader::ANIMATION_TYPE::ATTACK);
	}
	else if (getComponentFromKnownEntity<ecs::components::MoveStateComponent>(pSkeleton->getEntityID()))
	{
		pSkeleton->skeletonData.UpdateAnimation(delta, ModelLoader::ANIMATION_TYPE::MOVE);
	}
	else
	{ 
		pSkeleton->skeletonData.UpdateAnimation(delta, ModelLoader::ANIMATION_TYPE::PING);
	}
}
