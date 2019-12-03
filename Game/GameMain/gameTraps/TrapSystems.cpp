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



ecs::systems::SpringRetractionSystem::SpringRetractionSystem()
{
	updateType = ecs::EntityUpdate;
	typeFilter.addRequirement(ecs::components::SpringRetractionComponent::typeID);
	typeFilter.addRequirement(ecs::components::TransformComponent::typeID);
}

ecs::systems::SpringRetractionSystem::~SpringRetractionSystem()
{
}

void ecs::systems::SpringRetractionSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	SpringRetractionComponent* p_sr_comp = _entityInfo.getComponent<components::SpringRetractionComponent>();
	TransformComponent* p_transf_comp = _entityInfo.getComponent<components::TransformComponent>();

	//if (p_sr_comp)
	//{
	//	p_sr_comp->mElapsedTime += _delta;

	//	if (p_sr_comp->mElapsedTime >= p_sr_comp->mDuration)
	//	{
	//		removeComponent(p_sr_comp->getEntityID(), p_sr_comp->getTypeID());	
	//	}
	//	else
	//	{
	//		TransformComponent* p_transf_comp = _entityInfo.getComponent<components::TransformComponent>();

	//		if (p_transf_comp)
	//		{
	//			float retraction_dist = (_delta / p_sr_comp->mDuration) * 3.0f;
	//			p_transf_comp->position.y -= retraction_dist;
	//		}
	//	}
	//}

	if (p_sr_comp)
	{
		const float distance = p_transf_comp->position.y - p_sr_comp->TargetOffsetY;
		if (distance <= 0.0f)
		{
			p_transf_comp->position.y = p_sr_comp->TargetOffsetY;
			removeComponent(p_sr_comp->getEntityID(), p_sr_comp->getTypeID());
		}
		else
		{
			const float speed = _delta * distance / max(p_sr_comp->mDuration, 0.01f);

			p_transf_comp->position.y	-= speed;
			p_sr_comp->mDuration		-= _delta;
		}

		/*p_transf_comp->position.y -= 1.f * _delta;

		if (p_transf_comp->position.y < p_sr_comp->TargetOffsetY)
		{
			p_transf_comp->position.y = p_sr_comp->TargetOffsetY;
			removeComponent(_entityInfo.entity->getID(), SpringRetractionComponent::typeID);
		}*/
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
		//TypeID id = dynamic_cast<TriggerFireTrapEvent*>(&event)->unitID;

		//if (id > 0)
		//{

		//	// Make the unit take damage
		//	HealthComponent* p_hp_comp = getComponentFromKnownEntity<HealthComponent>(id);
		//	if (p_hp_comp)
		//	{
		//		p_hp_comp->mHealth -= mDamage * delta;

		//		// Make the unit jump a litte, fire is hot and so am I
		//		ForceImpulseEvent knockback;
		//		knockback.mDirection = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		//		knockback.mForce = mKnockback;
		//		knockback.mEntityID = id;
		//		createEvent(knockback);


		//		// Check if the unit died
		//		if (p_hp_comp->mHealth <= 0.0f)
		//		{
		//			ecs::components::DeadComponent dead_comp;
		//			ecs::ECSUser::createComponent(id, dead_comp);
		//		}
		//	}
		//}

		const float trap_damage = 20.0f;

		const ID tile_id = static_cast<TriggerFireTrapEvent&>(event).tileID;
		if (tile_id > 0)
		{
			TransformComponent* p_transf_comp = getComponentFromKnownEntity<TransformComponent>(tile_id);

			if (!p_transf_comp) return;

			const XMVECTOR tile_position = XMLoadFloat3(&p_transf_comp->position);

			TypeFilter unit_filter;
			unit_filter.addRequirement(components::UnitComponent::typeID);
			unit_filter.addRequirement(components::TransformComponent::typeID);
			EntityIterator units = getEntitiesByFilter(unit_filter);
			for (FilteredEntity& unit : units.entities)
			{
				const XMVECTOR unit_position = XMLoadFloat3(&unit.getComponent<components::TransformComponent>()->position);
				const ID unit_id = unit.entity->getID();

				const float dist = XMVectorGetX(XMVector3Length(unit_position - tile_position));
				if (dist <= 1.0f)
				{
					// Make the unit take damage
					HealthComponent* p_hp_comp = getComponentFromKnownEntity<HealthComponent>(unit_id);
					if (p_hp_comp)
					{
						p_hp_comp->mHealth -= trap_damage;// mDamage* delta;

						// Make the unit jump a litte, fire is hot and so am I
						ForceImpulseEvent knockback;
						knockback.mDirection = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
						knockback.mForce = mKnockback;
						knockback.mEntityID = unit_id;
						createEvent(knockback);


						// Check if the unit died
						if (p_hp_comp->mHealth <= 0.0f)
						{
							ecs::components::DeadComponent dead_comp;
							ecs::ECSUser::createComponent(unit_id, dead_comp);
						}
					}
				}
			}

			/* Spawn Smoke Emitter At Sword Spawn */
			components::ParticleSpawnerComponent spawner;
			components::FireSpawnerComponent smoke;


			spawner.StartPosition = p_transf_comp->position;
			spawner.StartPosition.y -= 1.0f;
			spawner.SpawnFrequency = 0.005f;
			spawner.TimerSinceLastSpawn = 0.0f;
			spawner.LifeDuration = 0.4f;

			smoke.InitialVelocity = 12.0f;
			smoke.SpawnCount = 500;

			createEntity(spawner, smoke);
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
	//if (event.getTypeID() == ecs::events::TriggerSpringTrapEvent::typeID)
	//{
	//	TypeID id = dynamic_cast<TriggerSpringTrapEvent*>(&event)->unitID;

	//	if (id > 0)
	//	{
	//		GridProp* p_gp = GridProp::GetInstance();

	//		// Loop over random tiles until a valid spot is found
	//		TransformComponent* start_trans_comp;
	//		TransformComponent* target_trans_comp;
	//		int2 area_size = p_gp->GetSize();

	//		int rand_x = (rand() % (area_size.x / 2)) + (area_size.x / 4);
	//		int rand_y = (rand() % (area_size.y / 2)) + (area_size.y / 4);
	//		while (!p_gp->mGrid[rand_x][rand_y].isPassable)
	//		{
	//			rand_x = (rand() % (area_size.x / 2)) + (area_size.x / 4);
	//			rand_y = (rand() % (area_size.y / 2)) + (area_size.y / 4);
	//		}

	//		target_trans_comp = getComponentFromKnownEntity<TransformComponent>(p_gp->mGrid[rand_x][rand_y].Id);

	//		// units transformcomponent
	//		start_trans_comp = getComponentFromKnownEntity<TransformComponent>(id);

	//		start_trans_comp->position.y += 3.0f;

	//		XMFLOAT2 flight_direction;
	//		flight_direction.x = target_trans_comp->position.x - start_trans_comp->position.x;
	//		flight_direction.y = target_trans_comp->position.z - start_trans_comp->position.z;

	//		float dist = sqrtf(flight_direction.x * flight_direction.x + flight_direction.y * flight_direction.y);

	//		flight_direction.x /= dist;
	//		flight_direction.y /= dist;

	//		// Make the unit jump a litte, fire is hot and so am I
	//		ForceImpulseEvent knockback;
	//		knockback.mDirection = DirectX::XMFLOAT3(flight_direction.x, 3.0f, flight_direction.y);
	//		knockback.mForce = 150;
	//		knockback.mEntityID = id;
	//		createEvent(knockback);


	//		// Send the tile up

	//		TypeID tileID = dynamic_cast<TriggerSpringTrapEvent*>(&event)->tileID;
	//			
	//		TransformComponent* p_tile_transf = getComponentFromKnownEntity<TransformComponent>(tileID);

	//		p_tile_transf->position.y += 2.95f;

	//		// Create a component to have the tile get lowered to the original space
	//		SpringRetractionComponent p_sr_comp;
	//		p_sr_comp.mDuration = 3.0f;

	//		createComponent(tileID,p_sr_comp);
	//	}
	//}

	if (event.getTypeID() == ecs::events::TriggerSpringTrapEvent::typeID)
	{
		const float knockback_force = 100.0f;

		TypeID tile_id = static_cast<TriggerSpringTrapEvent&>(event).tileID;

		if (tile_id < 1) return;

		TransformComponent* p_tile_transf = getComponentFromKnownEntity<TransformComponent>(tile_id);
		const XMVECTOR tile_position = XMLoadFloat3(&p_tile_transf->position);

		TypeFilter unit_filter;
		unit_filter.addRequirement(components::UnitComponent::typeID);
		unit_filter.addRequirement(components::TransformComponent::typeID);
		EntityIterator units = getEntitiesByFilter(unit_filter);
		for (FilteredEntity& unit : units.entities)
		{
			const XMVECTOR unit_position = XMLoadFloat3(&unit.getComponent<components::TransformComponent>()->position);
			const ID unit_id = unit.entity->getID();

			if (XMVectorGetX(XMVector3Length(unit_position - tile_position)) > 1.0f)
			{
				continue;
			}

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
			start_trans_comp = getComponentFromKnownEntity<TransformComponent>(unit_id);

			start_trans_comp->position.y += 1.5f;

			XMFLOAT2 flight_direction;
			flight_direction.x = target_trans_comp->position.x - start_trans_comp->position.x;
			flight_direction.y = target_trans_comp->position.z - start_trans_comp->position.z;

			float dist = sqrtf(flight_direction.x * flight_direction.x + flight_direction.y * flight_direction.y);

			flight_direction.x /= dist;
			flight_direction.y /= dist;  

			// Make the unit jump a litte, fire is hot and so am I
			ForceImpulseEvent knockback;
			knockback.mDirection = DirectX::XMFLOAT3(flight_direction.x, 3.0f, flight_direction.y);
			knockback.mForce = knockback_force;
			knockback.mEntityID = unit_id;
			createEvent(knockback);

		}

		// Create a component to have the tile get lowered to the original space
		SpringRetractionComponent p_sr_comp;
		p_sr_comp.mDuration = 2.0f;
		p_sr_comp.TargetOffsetY = p_tile_transf->position.y;

		createComponent(tile_id, p_sr_comp);

		// Send the tile up
		p_tile_transf->position.y += 1.0f;
	}
}

ecs::systems::SpikeTrapEventSystem::SpikeTrapEventSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(ecs::events::TriggerSpikeTrapEvent::typeID);
}

ecs::systems::SpikeTrapEventSystem::~SpikeTrapEventSystem()
{
	//
}

void ecs::systems::SpikeTrapEventSystem::readEvent(BaseEvent& event, float delta)
{
	if (event.getTypeID() != ecs::events::TriggerSpikeTrapEvent::typeID)
	{
		return;
	}

	constexpr float trap_damage		= 20.0f;
	constexpr float trap_offset_y	= 0.25f;
	constexpr float trap_force		= 50.0f;

	ecs::events::TriggerSpikeTrapEvent& r_event = static_cast<ecs::events::TriggerSpikeTrapEvent&>(event);
	
	// Handle damage on unit

	TypeID trap_id = r_event.trapID;
	TypeID tile_id = r_event.tileID;


	if (trap_id < 1 || tile_id < 1) return;

	TransformComponent* p_trap_transf = getComponentFromKnownEntity<TransformComponent>(trap_id);
	TransformComponent* p_tile_transf = getComponentFromKnownEntity<TransformComponent>(tile_id);
	const XMVECTOR tile_position = XMLoadFloat3(&p_tile_transf->position);


	// Create a component to have the tile get lowered to the original space
	SpringRetractionComponent sr_comp;
	sr_comp.TargetOffsetY	= p_trap_transf->position.y;
	sr_comp.mDuration		= 10.0f;
	createComponent(trap_id, sr_comp);

	// Set new position
	p_trap_transf->position.y = p_tile_transf->position.y + trap_offset_y;


	TypeFilter unit_filter;
	unit_filter.addRequirement(components::UnitComponent::typeID);
	unit_filter.addRequirement(components::TransformComponent::typeID);
	EntityIterator units = getEntitiesByFilter(unit_filter);
	for (FilteredEntity& unit : units.entities)
	{
		const XMVECTOR unit_position = XMLoadFloat3(&unit.getComponent<components::TransformComponent>()->position);
		const ID unit_id = unit.entity->getID();

		const float dist = XMVectorGetX(XMVector3Length(unit_position - tile_position));
		if (dist <= 1.0f)
		{
			// Make the unit take damage
			HealthComponent* p_hp_comp = getComponentFromKnownEntity<HealthComponent>(unit_id);
			if (p_hp_comp)
			{
				p_hp_comp->mHealth -= trap_damage;

				// Check if the unit died
				if (p_hp_comp->mHealth <= 0.0f)
				{
					ecs::components::DeadComponent dead_comp;
					ecs::ECSUser::createComponent(unit_id, dead_comp);
				}
				else
				{
					// VISUAL
					ColorSwitchEvent damage_flash;
					damage_flash.mColor = WHITE;
					damage_flash.mEntityID = unit_id;
					damage_flash.mTime = 0.05f;
					createEvent(damage_flash);


					// Make the unit jump a litte because of sharp 
					ForceImpulseEvent knockback;
					knockback.mDirection = DirectX::XMFLOAT3(0, 1.0f, 0);
					knockback.mForce = trap_force;
					knockback.mEntityID = unit_id;
					createEvent(knockback);
				}
			}
		}
	}
}
