#pragma once
#include <ecsSystemIncludes.h>
#include <UtilityComponents.h>
#include <AIComponents.h>
#include <InputComponents.h>
#include "GameComponents.h"



namespace ecs
{
	namespace systems
	{
		class GameOverSystem : public ECSSystem<GameOverSystem>
		{
		public:
			GameOverSystem()
			{
				updateType = EntityUpdate;
				typeFilter.addRequirement(components::GameStateComponent::typeID);
			}
			virtual ~GameOverSystem() {}
			void updateEntity(FilteredEntity& entity, float delta) override
			{
				int test = 0;
				//components::KeyboardComponent* kbComp = entity.getComponent<components::KeyboardComponent>();
				components::GameStateComponent* gsComp = entity.getComponent<components::GameStateComponent>();
				
				components::KeyboardComponent* kbComp = ((components::KeyboardComponent*)this->getComponentsOfType(components::KeyboardComponent::typeID).next());

				if (kbComp->R)
				{
					gsComp->gameState = 666;
				}
			}
		};

	}
}