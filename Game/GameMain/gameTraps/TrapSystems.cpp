#include "TrapSystems.h"
#include "../Physics/PhysicsComponents.h"
#include "../Physics/PhysicsEvents.h"
#include "../gameAnimation/AnimationComponents.h"
#include "../gameAI/AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "GridProp.h"
#include "../gameGraphics/ParticleECSComponents.h"


/*
------------------------------------------------------------
------------------------------------------------------------
------------------------------------------------------------
------------------- UPDATE SYSTEMS -------------------------
------------------------------------------------------------
------------------------------------------------------------
------------------------------------------------------------
*/


ecs::systems::FreezingDurationSystem::FreezingDurationSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::FreezingTimerComponent::typeID);
	typeFilter.addRequirement(ecs::components::HealthComponent::typeID);
}

ecs::systems::FreezingDurationSystem::~FreezingDurationSystem()
{
}

void ecs::systems::FreezingDurationSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	FreezingTimerComponent* p_ftimer_comp = _entityInfo.getComponent<components::FreezingTimerComponent>();
	HealthComponent* p_hp_comp = _entityInfo.getComponent<components::HealthComponent>();

	if (p_ftimer_comp)
	{
		p_ftimer_comp->mElapsedTime += _delta;

		if (p_ftimer_comp->mElapsedTime >= p_ftimer_comp->mDuration)
		{
			TypeID id = _entityInfo.entity->getID();
			if (id > 0)
			{
				//components::HealthComponent* p_hp_comp = getComponentFromKnownEntity<HealthComponent>(id);
				if (p_hp_comp)
				{
					components::DynamicMovementComponent* p_move_comp = getComponentFromKnownEntity<DynamicMovementComponent>(id);
					components::AnimationSpeedComponent* p_ani_speed_comp = getComponentFromKnownEntity<AnimationSpeedComponent>(id);

					p_move_comp->mMaxVelocity *= 2.0f;
					p_ani_speed_comp->factor *= 2.0f;
				}

				removeComponent(p_ftimer_comp->getEntityID(), p_ftimer_comp->getTypeID());
			}

		}
	}
	
}


/*
------------------------------------------------------------
------------------------------------------------------------
------------------------------------------------------------
------------------- EVENT READERS --------------------------
------------------------------------------------------------
------------------------------------------------------------
------------------------------------------------------------
*/




ecs::systems::FireTrapEventSystem::FireTrapEventSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(ecs::events::TriggerFireTrapEvent::typeID);
}

ecs::systems::FireTrapEventSystem::~FireTrapEventSystem()
{
}

void ecs::systems::FireTrapEventSystem::readEvent(BaseEvent& event, float delta)
{
	if (event.getTypeID() == ecs::events::TriggerFireTrapEvent::typeID)
	{
		TypeID id = dynamic_cast<TriggerFireTrapEvent*>(&event)->unitID;

		if (id > 0)
		{

			// Make the unit take damage
			HealthComponent* p_hp_comp = getComponentFromKnownEntity<HealthComponent>(id);
			if (p_hp_comp)
			{
				p_hp_comp->mHealth -= mDamage;

				// Make the unit jump a litte, fire is hot and so am I
				ForceImpulseEvent knockback;
				knockback.mDirection = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
				knockback.mForce = mKnockback;
				knockback.mEntityID = id;
				createEvent(knockback);

				/* Spawn Smoke Emitter At Sword Spawn */
				components::ParticleSpawnerComponent spawner;
				components::FireSpawnerComponent smoke;


				TransformComponent* p_transf_comp = getComponentFromKnownEntity<TransformComponent>(id);

				spawner.StartPosition = p_transf_comp->position;
				spawner.StartPosition.y -=  1.0f;
				spawner.SpawnFrequency = 0.005f;
				spawner.TimerSinceLastSpawn = 0.0f;
				spawner.LifeDuration = 0.4f;

				smoke.InitialVelocity = 12.0f;
				smoke.SpawnCount = 150;

				createEntity(spawner, smoke);


				// Check if the unit died
				if (p_hp_comp->mHealth <= 0.0f)
				{
					ecs::components::DeadComponent dead_comp;
					ecs::ECSUser::createComponent(id, dead_comp);
				}
			}
		}
	}
}

ecs::systems::FreezeTrapEventSystem::FreezeTrapEventSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(ecs::events::TriggerFreezeTrapEvent::typeID);
}

ecs::systems::FreezeTrapEventSystem::~FreezeTrapEventSystem()
{
}

void ecs::systems::FreezeTrapEventSystem::readEvent(BaseEvent& event, float delta)
{
	if (event.getTypeID() == ecs::events::TriggerFreezeTrapEvent::typeID)
	{
		TypeID id = dynamic_cast<TriggerFreezeTrapEvent*>(&event)->unitID;

		if (id > 0)
		{
			HealthComponent* p_hp_comp = getComponentFromKnownEntity<HealthComponent>(id);
			if (p_hp_comp)
			{
				p_hp_comp->mHealth -= mDamage;

				// Check if the unit died
				if (p_hp_comp->mHealth <= 0.0f)
				{
					ecs::components::DeadComponent dead_comp;
					ecs::ECSUser::createComponent(id, dead_comp);
				}
				else
				{
					components::UnitComponent* p_unit_comp = getComponentFromKnownEntity<UnitComponent>(id);
					components::DynamicMovementComponent* p_move_comp = getComponentFromKnownEntity<DynamicMovementComponent>(id);
					components::AnimationSpeedComponent* p_ani_speed_comp = getComponentFromKnownEntity<AnimationSpeedComponent>(id);

					p_move_comp->mMaxVelocity *= mPower;
					p_ani_speed_comp->factor = mPower;

					// Add component 
					FreezingTimerComponent f_comp;
					f_comp.mDuration = 3.0f;
					f_comp.mElapsedTime = 0.0f;

					createComponent(id, f_comp);

					// Make them brigther for the duration
					events::ColorSwitchEvent eve;
					eve.mEntityID = id;
					eve.mTime = f_comp.mDuration;
					Color color = Color(0, 0, 0);
					switch (p_unit_comp->playerID)
					{
					case PLAYER1:
						color = BRIGHT_RED;
						break;
					case PLAYER2:
						color = BRIGHT_PURPLE;
						break;
					case PLAYER3:
						color = BRIGHT_BLUE;
						break;
					case PLAYER4:
						color = BRIGHT_GREEN;
						break;
					}
					eve.mColor = color;

					createEvent(eve);
				}
			}
		}
	}
}

ecs::systems::SpringTrapEventSystem::SpringTrapEventSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(ecs::events::TriggerSpringTrapEvent::typeID);
}

ecs::systems::SpringTrapEventSystem::~SpringTrapEventSystem()
{
}

void ecs::systems::SpringTrapEventSystem::readEvent(BaseEvent& event, float delta)
{
	if (event.getTypeID() == ecs::events::TriggerSpringTrapEvent::typeID)
	{
		TypeID id = dynamic_cast<TriggerSpringTrapEvent*>(&event)->unitID;

		if (id > 0)
		{
			GridProp* p_gp = GridProp::GetInstance();

			// Loop over random tiles until a valid spot is found
			TransformComponent* start_trans_comp;
			TransformComponent* target_trans_comp;
			int2 area_size = p_gp->GetSize();

			int rand_x = (rand() % (area_size.x / 2)) + (area_size.x / 4);
			int rand_y = (rand() % (area_size.y / 2)) + (area_size.y / 4);
			while (!p_gp->mGrid[rand_x][rand_y].isPassable)
			{
				rand_x = (rand() % (area_size.x / 2)) + (area_size.x / 4);
				rand_y = (rand() % (area_size.y / 2)) + (area_size.y / 4);
			}

			target_trans_comp = getComponentFromKnownEntity<TransformComponent>(p_gp->mGrid[rand_x][rand_y].Id);

			// units transformcomponent
			start_trans_comp = getComponentFromKnownEntity<TransformComponent>(id);


			XMFLOAT2 flight_direction;
			flight_direction.x = target_trans_comp->position.x - start_trans_comp->position.x;
			flight_direction.y = target_trans_comp->position.z - start_trans_comp->position.z;

			float dist = sqrtf(flight_direction.x * flight_direction.x + flight_direction.y * flight_direction.y);

			flight_direction.x /= dist;
			flight_direction.y /= dist;

			// Make the unit jump a litte, fire is hot and so am I
			ForceImpulseEvent knockback;
			knockback.mDirection = DirectX::XMFLOAT3(flight_direction.x, 3.0f, flight_direction.y);
			knockback.mForce = 150;
			knockback.mEntityID = id;
			createEvent(knockback);
		}
	}
}
