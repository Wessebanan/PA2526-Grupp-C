#include "TrapSystems.h"
#include "../Physics/PhysicsComponents.h"
#include "../Physics/PhysicsEvents.h"
#include "../gameAnimation/AnimationComponents.h"
#include "../gameAI/AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "GridProp.h"
#include "../gameGraphics/ParticleECSComponents.h"
#include "..//gameUtility/UtilityEcsFunctions.h"

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
ecs::systems::GenericTrapEventSystem::GenericTrapEventSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(ecs::events::TriggerTrapEvent::typeID);
}

ecs::systems::GenericTrapEventSystem::~GenericTrapEventSystem()
{

}

void ecs::systems::GenericTrapEventSystem::readEvent(BaseEvent& event, float delta)
{
	TriggerTrapEvent* p_event = static_cast<TriggerTrapEvent*>(&event);
	GAME_OBJECT_TYPE type = p_event->trapType;

	switch (type)
	{
	case GAME_OBJECT_TYPE_TRAP_FIRE:
	{
		TriggerFireTrapEvent fire_event;
		fire_event.trapID = p_event->trapID;
		fire_event.tileID = p_event->tileID;
		fire_event.unitID = p_event->unitID;
		createEvent(fire_event);
		break;
	}
	case GAME_OBJECT_TYPE_TRAP_SPIKES:
	{
		TriggerSpikeTrapEvent spike_event;
		spike_event.trapID = p_event->trapID;
		spike_event.tileID = p_event->tileID;
		spike_event.unitID = p_event->unitID;
		createEvent(spike_event);
		break;
	}
	case GAME_OBJECT_TYPE_TRAP_SPRING:
	{
		TriggerSpringTrapEvent spring_event;
		spring_event.trapID = p_event->trapID;
		spring_event.tileID = p_event->tileID;
		spring_event.unitID = p_event->unitID;
		createEvent(spring_event);
		break;
	}
	case GAME_OBJECT_TYPE_TRAP_FREEZE:
	{
		TriggerFreezeTrapEvent freeze_event;
		freeze_event.trapID = p_event->trapID;
		freeze_event.tileID = p_event->tileID;
		freeze_event.unitID = p_event->unitID;
		createEvent(freeze_event);
		break;
	}
	}
}

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
#pragma region old_shit
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
#pragma endregion
		const float trap_damage = 20.0f;

		TriggerFireTrapEvent& fire_trap_event = static_cast<TriggerFireTrapEvent&>(event);
		
		if (fire_trap_event.tileID > 0)
		{
			TransformComponent* p_trap_transform = getComponentFromKnownEntity<TransformComponent>(fire_trap_event.trapID);
			TrapComponent* p_trap_component = getComponentFromKnownEntity<TrapComponent>(fire_trap_event.trapID);

			if (!p_trap_transform) return;
			if (!p_trap_component) return;

			// Transforming a copy of the trap aabb to world space.
			XMMATRIX trap_world = UtilityEcsFunctions::GetWorldMatrix(*p_trap_transform);
			AABB trap_aabb_copy = AABB(p_trap_component->mAABB);

			trap_aabb_copy.Transform(trap_world);

			TypeFilter unit_filter;
			unit_filter.addRequirement(components::UnitComponent::typeID);
			unit_filter.addRequirement(components::TransformComponent::typeID);
			unit_filter.addRequirement(components::ObjectCollisionComponent::typeID);
			EntityIterator units = getEntitiesByFilter(unit_filter);
			for (FilteredEntity& unit : units.entities)
			{				
				const ID unit_id = unit.entity->getID();

				// Transforming a copy of the unit bv to world space.
				ObjectCollisionComponent* p_collision = unit.getComponent<ObjectCollisionComponent>();
				TransformComponent* p_transform = unit.getComponent<TransformComponent>();
				XMMATRIX unit_world = UtilityEcsFunctions::GetWorldMatrix(*p_transform);

				BoundingVolume* p_bv_copy = p_collision->mBV->Copy();
				p_bv_copy->Transform(unit_world);

				// Hit if unit collides with trap.
				if (p_bv_copy->Intersects(&trap_aabb_copy))
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
				delete p_bv_copy;
			}

			/* Spawn Smoke Emitter At Sword Spawn */
			components::ParticleSpawnerComponent spawner;
			components::FireSpawnerComponent smoke;

			spawner.StartPosition = p_trap_transform->position;
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
#pragma region old_shit
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
#pragma endregion
	if (event.getTypeID() == ecs::events::TriggerSpringTrapEvent::typeID)
	{
		const float knockback_force = 100.0f;

		TriggerSpringTrapEvent& r_event = static_cast<TriggerSpringTrapEvent&>(event);

		if (r_event.tileID < 1) return;

		TransformComponent* p_tile_transform = getComponentFromKnownEntity<TransformComponent>(r_event.tileID);
		TransformComponent* p_trap_transform = getComponentFromKnownEntity<TransformComponent>(r_event.trapID);
		TrapComponent* p_trap_component = getComponentFromKnownEntity<TrapComponent>(r_event.trapID);

		if (!p_trap_transform) return;
		if (!p_trap_component) return;

		// Transforming a copy of the trap aabb to world space.
		XMMATRIX trap_world = UtilityEcsFunctions::GetWorldMatrix(*p_trap_transform);
		AABB trap_aabb_copy = AABB(p_trap_component->mAABB);

		trap_aabb_copy.Transform(trap_world);

		TypeFilter unit_filter;
		unit_filter.addRequirement(components::UnitComponent::typeID);
		unit_filter.addRequirement(components::TransformComponent::typeID);
		unit_filter.addRequirement(components::ObjectCollisionComponent::typeID);
		EntityIterator units = getEntitiesByFilter(unit_filter);
		for (FilteredEntity& unit : units.entities)
		{
			// Transforming a copy of the unit bv to world space.
			ObjectCollisionComponent* p_collision = unit.getComponent<ObjectCollisionComponent>();
			TransformComponent* p_transform = unit.getComponent<TransformComponent>();
			XMMATRIX unit_world = UtilityEcsFunctions::GetWorldMatrix(*p_transform);

			BoundingVolume* p_bv_copy = p_collision->mBV->Copy();
			p_bv_copy->Transform(unit_world);

			// Hit if unit collides with trap.
			if (p_bv_copy->Intersects(&trap_aabb_copy))
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
				start_trans_comp = unit.getComponent<TransformComponent>();

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
				knockback.mEntityID = unit.entity->getID();
				createEvent(knockback);
			}
			delete p_bv_copy;
		}

		// Create a component to have the tile get lowered to the original space
		SpringRetractionComponent p_sr_comp;
		p_sr_comp.mDuration = 2.0f;
		p_sr_comp.TargetOffsetY = p_tile_transform->position.y;

		createComponent(r_event.tileID, p_sr_comp);

		// Send the tile up
		p_tile_transform->position.y += 1.0f;
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

	ID trap_id = r_event.trapID;
	ID tile_id = r_event.tileID;


	if (trap_id < 1 || tile_id < 1) return;

	TransformComponent* p_tile_transform = getComponentFromKnownEntity<TransformComponent>(tile_id);
	TransformComponent* p_trap_transform = getComponentFromKnownEntity<TransformComponent>(trap_id);
	TrapComponent* p_trap_component = getComponentFromKnownEntity<TrapComponent>(trap_id);
	
	if (!p_tile_transform) return;
	if (!p_trap_transform) return;
	if (!p_trap_component) return;

	// Create a component to have the tile get lowered to the original space
	SpringRetractionComponent sr_comp;
	sr_comp.TargetOffsetY	= p_trap_transform->position.y;
	sr_comp.mDuration		= 10.0f;
	createComponent(trap_id, sr_comp);

	// Set new position
	p_trap_transform->position.y = p_tile_transform->position.y + trap_offset_y;

	// Transforming a copy of the trap aabb to world space.
	XMMATRIX trap_world = UtilityEcsFunctions::GetWorldMatrix(*p_trap_transform);
	AABB trap_aabb_copy = AABB(p_trap_component->mAABB);

	trap_aabb_copy.Transform(trap_world);

	TypeFilter unit_filter;
	unit_filter.addRequirement(components::UnitComponent::typeID);
	unit_filter.addRequirement(components::TransformComponent::typeID);
	unit_filter.addRequirement(components::ObjectCollisionComponent::typeID);
	EntityIterator units = getEntitiesByFilter(unit_filter);
	for (FilteredEntity& unit : units.entities)
	{
		// Transforming a copy of the unit bv to world space.
		ObjectCollisionComponent* p_collision = unit.getComponent<ObjectCollisionComponent>();
		TransformComponent* p_transform = unit.getComponent<TransformComponent>();
		XMMATRIX unit_world = UtilityEcsFunctions::GetWorldMatrix(*p_transform);

		BoundingVolume* p_bv_copy = p_collision->mBV->Copy();
		p_bv_copy->Transform(unit_world);

		// Hit if unit collides with trap.
		if (p_bv_copy->Intersects(&trap_aabb_copy))
		{
			// Make the unit take damage
			HealthComponent* p_hp_comp = getComponentFromKnownEntity<HealthComponent>(unit.entity->getID());
			if (p_hp_comp)
			{
				p_hp_comp->mHealth -= trap_damage;

				// Check if the unit died
				if (p_hp_comp->mHealth <= 0.0f)
				{
					ecs::components::DeadComponent dead_comp;
					ecs::ECSUser::createComponent(unit.entity->getID(), dead_comp);
				}
				else
				{
					// VISUAL
					ColorSwitchEvent damage_flash;
					damage_flash.mColor = WHITE;
					damage_flash.mEntityID = unit.entity->getID();
					damage_flash.mTime = 0.05f;
					createEvent(damage_flash);

					// Make the unit jump a litte because of sharp 
					ForceImpulseEvent knockback;
					knockback.mDirection = DirectX::XMFLOAT3(0, 1.0f, 0);
					knockback.mForce = trap_force;
					knockback.mEntityID = unit.entity->getID();
					createEvent(knockback);
				}
			}
		}
		delete p_bv_copy;
	}
}
