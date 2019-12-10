#include "TrapSpawnerSystems.h"
#include "TrapComponents.h"

#include "../gameUtility/UtilityComponents.h"
#include "../gameUtility/UtilityGraphics.h"
#include "../gameGameLoop/GameLoopEvents.h"
#include "../gameGraphics/ParticleECSComponents.h"
#include "../gameAudio/AudioECSEvents.h"

#include "AIGlobals.h"

using namespace DirectX;

#include <iostream>

namespace ecs
{
	namespace systems
	{
		TrapSpawnerSystem::TrapSpawnerSystem() : mDurationSinceLastSpawn(0)
		{
			updateType = Actor;
		}

		TrapSpawnerSystem::~TrapSpawnerSystem()
		{
			//
		}

		void TrapSpawnerSystem::act(float delta)
		{
			mDurationSinceLastSpawn += delta;
			if (mDurationSinceLastSpawn < mDurationBetweenSpawns)
			{
				return;
			}

			/*
				Reset counter in order to time for next spawn
			*/

			mDurationSinceLastSpawn -= mDurationBetweenSpawns;

			/*
				Randomize a trap in queue to spawn
			*/

			const int rand_index = rand() % (int)mQueue.size();

			components::TrapQueueInfoComponent* p_info =
				getComponent<components::TrapQueueInfoComponent>(mQueue[rand_index]);

			// Sanity check
			if (!p_info)
			{
				return;
			}

			/*
				-- Create trap BV
				Creates an AABB around the trap mesh for collision.
			*/
			std::vector<XMFLOAT3>* p_vertices = MeshContainer::GetMeshCPU(p_info->trapCompInfo.mObjectType)->GetVertexPositionVector();
			p_info->trapCompInfo.mAABB.CreateFromPoints(p_info->trapCompInfo.mAABB, p_vertices->size(), p_vertices->data(), sizeof(XMFLOAT3));

			// Boost height of spring BV slightly since it's a circle.
			if (p_info->trapCompInfo.mObjectType == GAME_OBJECT_TYPE_TRAP_SPRING)
			{
				p_info->trapCompInfo.mAABB.Extents.y += 0.1f;
			}

			createEntity(p_info->trapCompInfo, p_info->colorCompInfo, p_info->transfCompInfo);


			/*
				Spawn particles where the trap spawns, to make the spawn more visible for the player
			*/

			ParticleSpawnerComponent particle_spawner_comp;
			TrapParticleSpawnComponent trap_particle_spawner_comp;

			particle_spawner_comp.StartPosition = p_info->transfCompInfo.position;
			particle_spawner_comp.SpawnFrequency = 0.005f;
			particle_spawner_comp.TimerSinceLastSpawn = 0.0f;
			particle_spawner_comp.LifeDuration = 1.0f;

			trap_particle_spawner_comp.InitialVelocity = 10.f;
			trap_particle_spawner_comp.SpawnCount = 100.f;
			
			trap_particle_spawner_comp.red = p_info->particleColor.red;
			trap_particle_spawner_comp.green = p_info->particleColor.green;
			trap_particle_spawner_comp.blue = p_info->particleColor.blue;

			createEntity(particle_spawner_comp, trap_particle_spawner_comp);

			/*
				Play spawn sound for trap
			*/

			events::PlaySound sound_event;
			sound_event.audioName = AudioName::SOUND_trapspawn;
			sound_event.soundFlags = SoundFlags::SF_RANDOM_PITCH;
			createEvent(sound_event);

			/*
				Remove queue info entity
			*/

			removeEntity(p_info->getEntityID());
			mQueue.erase(mQueue.begin() + rand_index);

			/*
				Check if there are any more queued trap spawns.
				If not, create event that starts the battle and
				remove this system.
			*/

			if (!mQueue.size())
			{
				//ecs::events::RoundStartEvent eve;
				//createEvent(eve);

				RemoveSystem<TrapSpawnerSystem>();
			}
		}

		void TrapSpawnerSystem::Initialize(float totalSpawnDuration)
		{
			/*
				Take snapshot of all currently existing trap spawn queue components.
				If there aren't any, remove this system as there are no traps to
				spawn.
			*/

			ComponentIterator queue_components =
				getComponentsOfType(components::TrapQueueInfoComponent::typeID);

			BaseComponent* p_base = nullptr;
			while (p_base = queue_components.next())
			{
				mQueue.push_back(p_base->getID());
			}

			if (!mQueue.size())
			{
				ecs::events::RoundStartEvent eve;
				createEvent(eve);

				RemoveSystem<TrapSpawnerSystem>();
				return;
			}

			mDurationBetweenSpawns = totalSpawnDuration / (float)mQueue.size();
		}



		TrapQueueSystem::TrapQueueSystem()
		{
			updateType = EventReader;
			typeFilter.addRequirement(events::PlaceTrapEvent::typeID);
			typeFilter.addRequirement(events::StartTrapSpawnSequenceEvent::typeID);
		}

		TrapQueueSystem::~TrapQueueSystem()
		{
			//
		}

		void TrapQueueSystem::readEvent(BaseEvent& _event, float _delta)
		{
			if (_event.getTypeID() == events::PlaceTrapEvent::typeID)
			{
				HandleQueueUp(static_cast<events::PlaceTrapEvent&>(_event));
			}

			else if (_event.getTypeID() == events::StartTrapSpawnSequenceEvent::typeID)
			{
				HandleSpawnSequence(static_cast<events::StartTrapSpawnSequenceEvent&>(_event));
			}
		}

		void TrapQueueSystem::HandleQueueUp(events::PlaceTrapEvent& queue_event)
		{
			/*
				-- Sanity check the event's trap type input
				Never create a trap that isn't defined.
			*/

			if (queue_event.type <= GAME_OBJECT_TYPE_TRAP_OFFSET_TAG || queue_event.type > GAME_OBJECT_TYPE(GAME_OBJECT_TYPE_TRAP_OFFSET_TAG + TRAP_TYPE_COUNT))
			{
				return;
			}

			components::TransformComponent* p_tile_transf = getComponentFromKnownEntity<components::TransformComponent>(queue_event.tileID);

			/*
				-- Create queue component
				Entities with a TrapQueueInfoComponents is storing
				information about later trap entity spawning.
			*/

			components::TrapQueueInfoComponent queue_info;

			/*
				-- Set trap type and tileID
			*/

			queue_info.trapCompInfo.mObjectType = queue_event.type;
			queue_info.trapCompInfo.mTileID = queue_event.tileID;

			queue_info.trapCompInfo.ActivationRateInSeconds = 4.0f + ((rand() % 2));
			queue_info.trapCompInfo.CurrentTimeInSeconds = 0.0f;

			/*
				-- Set trap position
				Trap is located above the tile it's created on. Add an
				offset to be placed above it.
			*/

			queue_info.transfCompInfo.position = p_tile_transf->position;
			queue_info.transfCompInfo.position.y += 0.01f;

			queue_info.transfCompInfo.scale.x = 0.9;
			queue_info.transfCompInfo.scale.y = 0.9;
			queue_info.transfCompInfo.scale.z = 0.9;

			/*
				-- Set trap color
				For now, all traps use the same mesh. Only difference
				is the color of the trap.
			*/

			switch (queue_info.trapCompInfo.mObjectType)
			{
			case GAME_OBJECT_TYPE_TRAP_FIRE:
				queue_info.colorCompInfo = components::ColorComponent(40, 40, 40);
				break;

			case GAME_OBJECT_TYPE_TRAP_FREEZE:
				queue_info.colorCompInfo = components::ColorComponent(150, 150, 150);
				break;

			case GAME_OBJECT_TYPE_TRAP_SPRING:
				queue_info.colorCompInfo = components::ColorComponent(191, 128, 64);
				break;

			case GAME_OBJECT_TYPE_TRAP_SPIKES:
				queue_info.colorCompInfo = components::ColorComponent(50, 50, 50);
				queue_info.transfCompInfo.position.y -= 10.0f;
				break;

			default:
				queue_info.colorCompInfo = components::ColorComponent(); // Default constructor sets color to black.
				break;
			}

			/*
				Set color of trap spawn particles
			*/

			switch (queue_event.userID)
			{
			case 0: // Red
			{
				queue_info.particleColor.red	= PLAYER1_COLOR.r;
				queue_info.particleColor.green	= PLAYER1_COLOR.g;
				queue_info.particleColor.blue	= PLAYER1_COLOR.b;
				break;
			}

			case 1: // Purple
			{
				queue_info.particleColor.red	= PLAYER2_COLOR.r;
				queue_info.particleColor.green	= PLAYER2_COLOR.g;
				queue_info.particleColor.blue	= PLAYER2_COLOR.b;
				break;
			}

			case 2: // Blue
			{
				queue_info.particleColor.red	= PLAYER3_COLOR.r;
				queue_info.particleColor.green	= PLAYER3_COLOR.g;
				queue_info.particleColor.blue	= PLAYER3_COLOR.b;
				break;
			}

			case 3: // Green
			{
				queue_info.particleColor.red	= PLAYER4_COLOR.r;
				queue_info.particleColor.green	= PLAYER4_COLOR.g;
				queue_info.particleColor.blue	= PLAYER4_COLOR.b;
				break;
			}

			default:
			{
				queue_info.particleColor.red = 255;
				queue_info.particleColor.green = 255;
				queue_info.particleColor.blue = 255;
				break;
			}
			}

			/*
				Queue up trap info. Spawning will be handled by TrapSpawnerSystem
			*/

			createEntity(queue_info);
		}

		void TrapQueueSystem::HandleSpawnSequence(events::StartTrapSpawnSequenceEvent& start_event)
		{
			TrapSpawnerSystem* p_system = (TrapSpawnerSystem*)CreateSystem<TrapSpawnerSystem>(0);

			/*
				Sanity check system creation.
				If the system already exist, CreateSystem() will return nullptr
			*/

			if (p_system)
			{
				p_system->Initialize(start_event.totalSpawnDuration);
			}
		}
	}
}