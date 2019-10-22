#include "FightingSystem.h"

#pragma region WeaponInitSystem
ecs::systems::WeaponInitSystem::WeaponInitSystem()
{
	updateType = EventListenerOnly;
	subscribeEventCreation(CreateComponentEvent::typeID);
}
ecs::systems::WeaponInitSystem::~WeaponInitSystem()
{

}
void ecs::systems::WeaponInitSystem::onEvent(TypeID _typeID, ecs::BaseEvent* _event)
{
	CreateComponentEvent* create_component_event = dynamic_cast<CreateComponentEvent*>(_event);

	// If the component created was any other than ground collision component, do nothing.
	if (create_component_event->componentTypeID != WeaponComponent::typeID)
	{
		return;
	}

	Entity* entity = getEntity(create_component_event->entityID);

	if (!entity->hasComponentOfType(MeshComponent::typeID))
	{
		return;
	}
	// Need transform component for scaling attack range.
	if (!entity->hasComponentOfType(TransformComponent::typeID))
	{
		return;
	}

	MeshComponent* mesh_component = getComponentFromKnownEntity<MeshComponent>(entity->getID());
	TransformComponent* transform_component = getComponentFromKnownEntity<TransformComponent>(entity->getID());
	WeaponComponent* weapon_component = getComponentFromKnownEntity<WeaponComponent>(entity->getID());
	std::vector<XMFLOAT3>* vertices = nullptr;

	// Fist has no mesh.
	if (weapon_component->mType != FIST)
	{
		vertices = mesh_component->mMesh->GetVertexPositionVector();
	}
	
	switch (weapon_component->mType)
	{
	// DirectXCollision is aggressive as f when making OBBs so I make
	// an AABB and then an OBB out of it to avoid a bunch of points being
	// outside of the OBB.
	case SWORD:
	{
		weapon_component->mBoundingVolume = new OBB;
		OBB* obb = static_cast<OBB*>(weapon_component->mBoundingVolume);
		AABB aabb;
		aabb.CreateFromPoints(aabb, vertices->size(), vertices->data(), sizeof(XMFLOAT3));
		obb->CreateFromBoundingBox(*(BoundingOrientedBox*)obb, aabb);

		// Applying scale to aabb to find actual range.
		aabb.Transform(UtilityEcsFunctions::GetWorldMatrix(*transform_component));

		// Finding greatest extent in obb and setting that (*2) to attack range (for now).
		XMFLOAT3 extents = aabb.Extents;
		weapon_component->mAttackRange = extents.x > extents.y ? (extents.x > extents.z ? extents.x : extents.z) : (extents.y > extents.z ? extents.y : extents.z);
		weapon_component->mAttackRange *= 2;
		weapon_component->mBaseDamage = BASE_SWORD_DAMAGE;
		break;
	}

	case FIST:
	{
		// Fist is only a unit sphere, supposed to get hand transform.
		weapon_component->mBoundingVolume = new Sphere;
		Sphere* sphere = static_cast<Sphere*>(weapon_component->mBoundingVolume);
		sphere->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
		sphere->Radius = 1.0f;
		
		// TODO: Get arm length and set to attack range.
		weapon_component->mAttackRange = 1.0f;

		weapon_component->mBaseDamage = BASE_FIST_DAMAGE;
		break;
	}
	default:
		break;
	}
}
#pragma endregion
#pragma region DamageSystem
ecs::systems::DamageSystem::DamageSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(WeaponComponent::typeID);
	typeFilter.addRequirement(TransformComponent::typeID);
}
ecs::systems::DamageSystem::~DamageSystem()
{

}
void ecs::systems::DamageSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	Entity* weapon = _entityInfo.entity;
	WeaponComponent* weapon_component = getComponentFromKnownEntity<WeaponComponent>(weapon->getID());
	TransformComponent* weapon_transform_component = getComponentFromKnownEntity<TransformComponent>(weapon->getID());

	// FILL OUT WITH OTHER WEAPONS LATER
	// Making a copy of the bounding volume for weapon.
	BoundingVolume* weapon_bv = nullptr;
	switch (weapon_component->mType)
	{
	case SWORD:
	{
		OBB* obb = static_cast<OBB*>(weapon_component->mBoundingVolume);
		weapon_bv = new OBB(*obb);
		break;
	}
	case FIST:
	{
		AABB* aabb = static_cast<AABB*>(weapon_component->mBoundingVolume);
		weapon_bv = new AABB(*aabb);
	}
	default:
		break;
	}
	
	// Transforming weapon bv to world space for collision.
	XMMATRIX weapon_world = UtilityEcsFunctions::GetWorldMatrix(*weapon_transform_component);
	weapon_bv->Transform(weapon_world);

	// Grab all entities with a constitution component (units).
	EntityIterator units = getEntitiesWithComponent<HealthComponent>();

	// Check collision against entities that could take damage.
	bool intersect = false;
	ID collided_unit = 0;

	for (int i = 0; i < units.entities.size(); i++)
	{
		ID current_unit = units.entities.at(i).entity->getID();
		// Skip weapon owner.
		if (current_unit == weapon_component->mOwnerEntity)
		{
			continue;
		}
		ObjectCollisionComponent* p_current_collision = getComponentFromKnownEntity<ObjectCollisionComponent>(current_unit);
		TransformComponent* p_current_transform = getComponentFromKnownEntity<TransformComponent>(current_unit);

		// Grabbing copy of AABB from current and transforming to world space.
		AABB current_aabb = p_current_collision->mAABB;
		XMMATRIX current_world_transform = UtilityEcsFunctions::GetWorldMatrix(*p_current_transform);
		current_aabb.BoundingBox::Transform(current_aabb, current_world_transform);		

		// Checking intersection and breaks if intersection.
		intersect = weapon_bv->Intersects(&current_aabb);
		if (intersect)
		{
			collided_unit = current_unit;
			break;
		}
	}
	
	// If a unit collides with an unowned weapon, set colliding unit to weapon owner.
	if (weapon_component->mOwnerEntity == 0)
	{
		weapon_component->mOwnerEntity = collided_unit;
		return;
	}

	if (intersect)
	{
		// Calculating velocity on weapon.
		float movement = CalculateDistance(weapon_component->mPreviousPos, weapon_transform_component->position);
		float velocity = movement / _delta;

		// Calculating damage by multiplying weapon velocity and the base damage.
		float damage = velocity * weapon_component->mBaseDamage;
		// s/t = v

		HealthComponent *collided_constitution = getComponentFromKnownEntity<HealthComponent>(collided_unit);

		collided_constitution->mHealth -= damage;

		// Deleting unit from existence if dead for now.
		if (collided_constitution->mHealth <= 0.0f)
		{
			removeEntity(collided_unit);
		}
	}
	
	weapon_component->mPreviousPos = weapon_transform_component->position;
}