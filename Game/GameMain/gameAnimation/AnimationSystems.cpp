#include "AnimationSystems.h"
#include "../gameAI/AIComponents.h"
ecs::systems::SkeletonSystem::SkeletonSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(components::SkeletonComponent::typeID);
	typeFilter.addRequirement(components::AnimationSpeedComponent::typeID);
}

ecs::systems::SkeletonSystem::~SkeletonSystem()
{
}

void ecs::systems::SkeletonSystem::updateEntity(FilteredEntity& entity, float delta)
{
	components::SkeletonComponent* pSkeleton = entity.getComponent<components::SkeletonComponent>();
	components::AnimationSpeedComponent* pAnimation = entity.getComponent<components::AnimationSpeedComponent>();
	if (!pSkeleton->ragdollEnabled)
	{
		if (pSkeleton->pingTimeElapsed >= 0.0f && pSkeleton->pingTimeElapsed <= 1.0f)
		{
			pSkeleton->pingTimeElapsed += delta;
			pSkeleton->skeletonData.UpdateAnimation(delta, ModelLoader::ANIMATION_TYPE::PING);
		}
		else if (getComponentFromKnownEntity<ecs::components::AttackStateComponent>(pSkeleton->getEntityID()))
		{
			pSkeleton->skeletonData.UpdateAnimation(delta, ModelLoader::ANIMATION_TYPE::ATTACK);
		}
		else if (getComponentFromKnownEntity<ecs::components::MoveStateComponent>(pSkeleton->getEntityID()))
		{
			pSkeleton->skeletonData.UpdateAnimation(delta * pAnimation->factor, ModelLoader::ANIMATION_TYPE::MOVE);
		}
		else
		{
			pSkeleton->skeletonData.UpdateAnimation(delta, ModelLoader::ANIMATION_TYPE::IDLE);
			// Uncomment the line below to test the ping animation as the "idle" animation
			//pSkeleton->skeletonData.UpdateAnimation(delta, ModelLoader::ANIMATION_TYPE::PING);
		}
	}
	else
	{
		pSkeleton->ragdoll.Resolve(delta);
		pSkeleton->ragdoll.RebuildHierarchy();
	}

}

void ecs::systems::PingListenerSystem::readEvent(BaseEvent& event, float delta)
{
	if (event.getTypeID() == ecs::events::PingEvent::typeID)
	{
		//Save values for easier use.
		PLAYER player = static_cast<ecs::events::PingEvent*>(&event)->playerId;
		//Find the correct player for the event.
		int i = 0;
		ecs::ComponentIterator it = ecs::ECSUser::getComponentsOfType(ecs::components::ArmyComponent::typeID);
		ecs::components::ArmyComponent* p_army = static_cast<ecs::components::ArmyComponent*>(it.next());
		while (i < player)
		{
			p_army = static_cast<ecs::components::ArmyComponent*>(it.next());
			i++;
		}
		//Loop through the players units and reset their pingTime.
		ecs::Entity* unit;
		for (int u = 0; u < p_army->unitIDs.size(); u++)
		{
			ID entity_id = p_army->unitIDs[u];
			unit = ecs::ECSUser::getEntity(entity_id);

			ecs::components::SkeletonComponent* unit_skeleton = ecs::ECSUser::getComponentFromKnownEntity<ecs::components::SkeletonComponent>(entity_id);
			unit_skeleton->pingTimeElapsed = 0.0f;
			unit_skeleton->skeletonData.ResetFrameCount();
		}
	}

}
