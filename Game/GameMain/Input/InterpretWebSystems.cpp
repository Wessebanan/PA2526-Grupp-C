#include "../Input/InterpretWebSystems.h"
#include "../gameAudio/AudioECSEvents.h"
#include "../gameAnimation/AnimationEvents.h"
#include "..//gameAI/AIComponents.h"

using namespace ecs;
using namespace ecs::components;

ecs::systems::ChangeFSMSystem::ChangeFSMSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::UserCommandComponent::typeID);
}

ecs::systems::ChangeFSMSystem::~ChangeFSMSystem()
{
}

void ecs::systems::ChangeFSMSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	UserCommandComponent* ucComp = _entityInfo.getComponent<UserCommandComponent>();
	ComponentIterator it = ecs::ECSUser::getComponentsOfType(PlayerStateComponent::typeID);
	PlayerStateComponent* p_player_state_comp = static_cast<PlayerStateComponent*>(it.next());

	if (ucComp)
	{
		for (size_t i = 0; i < 4; i++)
		{
			// CURRENTLY HIJACKED AS THE "PING" bBUTTON
			if (ucComp->userCommands[i].mCommand == "move" && p_player_state_comp->mCurrentStates[i] != STATE::MOVE)
			{
				// change state component
				events::PingEvent ping_event;
				ping_event.playerId = (PLAYER)i;

				p_player_state_comp->mCurrentStates[i] = STATE::MOVE;

				createEvent(ping_event);
			}
			else if (ucComp->userCommands[i].mCommand == "idle" )
			{
				// change state component
				events::ChangeUserStateEvent cus_event;
				cus_event.newState = STATE::IDLE;
				cus_event.playerId = (PLAYER)i;

				p_player_state_comp->mCurrentStates[i] = STATE::IDLE;

				{
					ecs::events::FadeOutSubMusic m_event;
					m_event.fadeOutTimeInSeconds = 3.0f;
					createEvent(m_event);
				}

				createEvent(cus_event);
			}
			else if (ucComp->userCommands[i].mCommand == "attack")
			{
				// change state component
				events::ChangeUserStateEvent cus_event;
				cus_event.newState = STATE::ATTACK;
				cus_event.playerId = (PLAYER)i;

				p_player_state_comp->mCurrentStates[i] = STATE::ATTACK;

				// TEMP GROUND HIT SOUND
				ecs::events::PlaySound sound_event;
				sound_event.audioName = AudioName::COIN_TEST_SOUND;
				sound_event.soundFlags = SoundFlags::SF_NONE;
				sound_event.invokerEntityId = _entityInfo.entity->getID();
				createEvent(sound_event);

				{
					ecs::events::FadeInSubMusic m_event;
					m_event.fadeInTimeInSeconds = 3.0f;
					createEvent(m_event);
				}

				createEvent(cus_event);
			}
			else if (ucComp->userCommands[i].mCommand == "loot" )
			{
				// change state component
				events::ChangeUserStateEvent cus_event;
				cus_event.newState = STATE::LOOT;
				cus_event.playerId = (PLAYER)i;

				p_player_state_comp->mCurrentStates[i] = STATE::LOOT;

				createEvent(cus_event);
			}
			else if (ucComp->userCommands[i].mCommand == "flee" )//&& p_player_state_comp->mCurrentStates[i] != STATE::FLEE)
			{
				// change state component
				events::ChangeUserStateEvent cus_event;
				cus_event.newState = STATE::FLEE;
				cus_event.playerId = (PLAYER)i;

				p_player_state_comp->mCurrentStates[i] = STATE::FLEE;

				createEvent(cus_event);
			}
		}
	}
}
