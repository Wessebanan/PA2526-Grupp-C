#include "../Input/InterpretWebSystems.h"
#include "../gameAudio/AudioECSEvents.h"
#include "../gameAnimation/AnimationEvents.h"
#include "..//gameAI/AIComponents.h"
#include "..//gameTraps//TrapEvents.h"
#include "GridProp.h"
#include "../gameUtility/UtilityComponents.h"

#include "InitInputBackendComponent.h"

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

ecs::systems::TrapEventSystem::TrapEventSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::UserButtonComponent::typeID);
	typeFilter.addRequirement(ecs::components::UserTileComponent::typeID);
	typeFilter.addRequirement(ecs::components::InputBackendComp::typeID);
}

ecs::systems::TrapEventSystem::~TrapEventSystem()
{
}

void ecs::systems::TrapEventSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	UserTileComponent* p_tile_comp = _entityInfo.getComponent<UserTileComponent>();
	UserButtonComponent* p_butt_comp = _entityInfo.getComponent<UserButtonComponent>();

	if (p_butt_comp && p_tile_comp)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (p_butt_comp->userButtons[i].mButton > -1)
			{
				if (p_tile_comp->userTiles[i].mCordX > -1)
				{

					GridProp* p_gp = GridProp::GetInstance();

					int2 gridSize = p_gp->GetSize();

					// Divide into 9 posible diffrent sections 
					int partionX = (gridSize.x - 6) / 3;
					int partionY = (gridSize.y - 6) / 3;

					int tileIndexX = ((p_tile_comp->userTiles[i].mCordX * partionX) + (rand() % partionX));
					int tileIndexY = ((p_tile_comp->userTiles[i].mCordY * partionY) + (rand() % partionY));
					tileIndexX += 3;
					tileIndexY += 3;

					TypeID tile_ID;
					
					int loops = 0;
					// Loop until we its a tile the units can go on
					while (!p_gp->mGrid[tileIndexX][tileIndexY].isPassable)
					{
						tileIndexX = (p_tile_comp->userTiles[i].mCordX * partionX) + (rand() % partionX);
						tileIndexY = (p_tile_comp->userTiles[i].mCordY * partionY) + (rand() % partionY);
						tileIndexX += 3;
						tileIndexY += 3;

						loops++;
					}
					tile_ID = p_gp->mGrid[tileIndexY][tileIndexX].Id;


					ecs::events::PlaceTrapEvent eve;
						
					eve.tileID = tile_ID;

					// Last part is falesafe if something would go wrong from the website
					eve.type = (TRAPTYPES)(p_butt_comp->userButtons[i].mButton % TRAPTYPES::TRAPCOUNT);

					createEvent(eve);

					// For debugging, remove when placing trap system is online
					TransformComponent* p_trans = getComponentFromKnownEntity<TransformComponent>(eve.tileID);
					if (p_trans)
					{
						p_trans->position.y = 5.0f;
					}

				
					InputBackendComp* p_backend = _entityInfo.getComponent<InputBackendComp>();

					p_backend->backend->resetUserButtonAndTile(i);
				}
			}
		}
		
	}
}
