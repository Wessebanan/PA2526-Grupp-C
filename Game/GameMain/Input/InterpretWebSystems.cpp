#include "../Input/InterpretWebSystems.h"
#include "../gameAudio/AudioECSEvents.h"
#include "../gameAnimation/AnimationEvents.h"
#include "..//gameAI/AIComponents.h"
#include "..//gameTraps//TrapEvents.h"
#include "GridProp.h"
#include "../gameUtility/UtilityComponents.h"

#include "InitInputBackendComponent.h"

#include "..//gameTraps/TrapEvents.h"
#include "..//gameTraps/TrapComponents.h"
#include "..\gameTraps\TrapSystems.h"

#include "..//UI/UISystems.h"

#include "..//Physics/PhysicsEvents.h"

using namespace ecs;
using namespace ecs::components;

ecs::systems::ChangeFSMSystem::ChangeFSMSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::UserCommandComponent::typeID);
	typeFilter.addRequirement(ecs::components::InputBackendComp::typeID);
}

ecs::systems::ChangeFSMSystem::~ChangeFSMSystem()
{
}

void ecs::systems::ChangeFSMSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	UserCommandComponent* ucComp = _entityInfo.getComponent<UserCommandComponent>();
	InputBackendComp* ib_comp = _entityInfo.getComponent<InputBackendComp>();
	ComponentIterator it = ecs::ECSUser::getComponentsOfType(PlayerStateComponent::typeID);
	PlayerStateComponent* p_player_state_comp = static_cast<PlayerStateComponent*>(it.next());

	if (ucComp)
	{
		for (size_t i = 0; i < 4; i++)
		{
			// CURRENTLY HIJACKED AS THE "PING" bBUTTON
			if (ucComp->userCommands[i].mCommand == "taunt"/* && p_player_state_comp->mCurrentStates[i] != STATE::TAUNT*/)
			{
				// change state component
				events::PingEvent ping_event;
				ping_event.playerId = (PLAYER)i;

				//p_player_state_comp->mCurrentStates[i] = STATE::TAUNT;

				createEvent(ping_event);


				// set the units to idel
				events::ChangeUserStateEvent eve;
				eve.newState = STATE::IDLE;
				eve.playerId = (PLAYER)i;
				createEvent(eve);

			}
			else if (ucComp->userCommands[i].mCommand == "regroup" )
			{
				// change state component
				events::ChangeUserStateEvent cus_event;
				cus_event.newState = STATE::RALLY;
				cus_event.playerId = (PLAYER)i;

				p_player_state_comp->mCurrentStates[i] = STATE::RALLY;

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

				// Attack button sound
				ecs::events::PlaySound sound_event;
				sound_event.audioName = AudioName::SOUND_coin;
				sound_event.soundFlags = SoundFlags::SF_NONE;
				sound_event.invokerEntityId = _entityInfo.entity->getID();
				//createEvent(sound_event);

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

				//// USED FOR TESTING DEBUGGING
				//events::ThrowUnitEvent trap_event;
				//trap_event.mTileID = (ID)(GridProp::GetInstance()->mGrid[9][16].Id);

				//ComponentIterator itt;
				//itt = getComponentsOfType<UnitComponent>();
				//trap_event.mUnitID = (ID)(UnitComponent*)itt.next()->getEntityID();
				//createEvent(trap_event);
				//trap_event.mUnitID = (TypeID)(UnitComponent*)itt.next()->getEntityID();
				//createEvent(trap_event);
				//trap_event.mUnitID = (TypeID)(UnitComponent*)itt.next()->getEntityID();
				//createEvent(trap_event);
			}
			else if (ucComp->userCommands[i].mCommand == "tutorial")//&& p_player_state_comp->mCurrentStates[i] != STATE::FLEE)
			{
				// Hax to have the player beeing able to send a command to start  the tutorial

				CreateSystem<UIGuideSystem>(1);
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
					int partion_x = (gridSize.x - 6) / 3;
					int partion_y = (gridSize.y - 6) / 3;

					int tile_index_x = ((p_tile_comp->userTiles[i].mCordX * partion_x) + (rand() % partion_x));
					int tile_index_y = ((p_tile_comp->userTiles[i].mCordY * partion_y) + (rand() % partion_y));
					tile_index_x += 3;
					tile_index_y += 3;
					
					ComponentIterator itt;
					
					// Takes the tilecomponent to make sure it isnt water
					TileComponent* p_map_tile = getComponentFromKnownEntity<TileComponent>(p_gp->mGrid[tile_index_y][tile_index_x].Id);

					int loops = 0;
					// Loop until we its a tile the units can go on, or 256 tries are made.
					while (((p_map_tile->tileType == TileTypes::WATER) || p_map_tile->impassable || p_map_tile->hasTrap) && loops < 256)
					{
						// Roll new tile
						tile_index_x = (p_tile_comp->userTiles[i].mCordX * partion_x) + (rand() % partion_x);
						tile_index_y = (p_tile_comp->userTiles[i].mCordY * partion_y) + (rand() % partion_y);
						tile_index_x += 3;
						tile_index_y += 3;

						p_map_tile = getComponentFromKnownEntity<TileComponent>(p_gp->mGrid[tile_index_y][tile_index_x].Id);

						loops++;
					}

					// If the tile is not valid OR not suitable for trap after 256 tries, skip.
					if (!p_map_tile || loops == 256)
					{
						return;
					}
					
					TypeID tile_ID = p_gp->mGrid[tile_index_y][tile_index_x].Id;

					p_map_tile->hasTrap = true;

					ecs::events::PlaceTrapEvent eve;
						
					eve.tileID = tile_ID;
					eve.userID = i;

					// Last part is falesafe if something would go wrong from the website
					// The 1 is to work around the index of GAME_OBJECT_TYPE_TRAP
					eve.type = (GAME_OBJECT_TYPES)(1 + p_butt_comp->userButtons[i].mButton + (int)GAME_OBJECT_TYPES::GAME_OBJECT_TYPE_TRAP_OFFSET_TAG);
					
					createEvent(eve);

					InputBackendComp* p_backend = _entityInfo.getComponent<InputBackendComp>();

					p_backend->backend->resetUserButtonAndTile(i);
					p_backend->mPlacedTraps[i]++;
					p_backend->backend->SendVibrate(i);
				}
			}
		}
		
	}
}
