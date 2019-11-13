#include "TrapSystems.h"
#include "../Physics/PhysicsComponents.h"
#include "../Physics/PhysicsEvents.h"
#include "../gameAI/AIComponents.h"
#include "../gameUtility/UtilityComponents.h"
#include "GridProp.h"

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

		// Make the unit take damage
		HealthComponent* p_hp_comp = getComponentFromKnownEntity<HealthComponent>(id);
		p_hp_comp->mHealth -= mDamage;


		// Make the unit jump a litte, fire is hot and so am I
		ForceImpulseEvent knockback;
		knockback.mDirection = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		knockback.mForce = mKnockback;
		knockback.mEntityID = id;
		createEvent(knockback);

		// Check if the unit died
		if (p_hp_comp->mHealth <= 0.0f)
		{
			ecs::components::DeadComponent dead_comp;
			ecs::ECSUser::createComponent(id, dead_comp);
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

		// So they cant run and mess up the flight to the next tile
		components::DynamicMovementComponent* p_movement_comp = getComponentFromKnownEntity<DynamicMovementComponent>(id);
		p_movement_comp->mMaxVelocity = 0.0f;
		
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
		knockback.mForce = dist * 10;
		knockback.mEntityID = id;
		createEvent(knockback);
	}
}
