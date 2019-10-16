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

	MeshComponent* mesh_component = getComponentFromKnownEntity<MeshComponent>(entity->getID());
	WeaponComponent* weapon_component = getComponentFromKnownEntity<WeaponComponent>(entity->getID());
	std::vector<XMFLOAT3> *vertices = mesh_component->mMesh->GetVertexPositionVector();

	switch (weapon_component->mType)
	{
		// DirectXCollision is aggressive as f when making OBBs so I make
		// an AABB and then an OBB out of it to avoid a bunch of points being
		// outside of the OBB.
	case SWORD:
	{
		weapon_component->mBoundingVolume = new OBB;
		OBB* obb = static_cast<OBB*>(weapon_component->mBoundingVolume);
		//weapon_component->mBo = new OBB;
		BoundingBox aabb;
		aabb.CreateFromPoints(aabb, vertices->size(), vertices->data(), sizeof(XMFLOAT3));
		obb->CreateFromBoundingBox(*(BoundingOrientedBox*)obb, aabb);
		break;
	}
	case PROJECTILE:
		break;
	case DEFAULT:
	{
		LPCSTR message = "Weapon component created without specified weapon type";
		MessageBoxA(NULL, message, NULL, MB_YESNO);
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
}
ecs::systems::DamageSystem::~DamageSystem()
{

}
void ecs::systems::DamageSystem::updateEntity(FilteredEntity& _entityInfo, float _delta)
{
	Entity* weapon = _entityInfo.entity;
	WeaponComponent* weapon_component = getComponentFromKnownEntity<WeaponComponent>(weapon->getID());
	TransformComponent* weapon_transform_component = getComponentFromKnownEntity<TransformComponent>(weapon->getID());
	// Check if weapon has an owner
	if (weapon_component->mOwnerEntity == 0)
	{
		return;
	}

	// Grab all entities with a constitution component (units).
	EntityIterator units = getEntitiesWithComponent<ConstitutionComponent>();

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
	default:
	{
		MessageBoxA(NULL, "Weapon Component has no type. (DamageSystem::updateEntity)", NULL, MB_YESNO);
		return;
	}
	}

	// Transforming weapon bv to world space for collision.
	XMMATRIX weapon_world = UtilityEcsFunctions::GetWorldMatrix(*weapon_transform_component);
	weapon_bv->Transform(weapon_world);

	// Check collision against entities that could take damage.
	bool intersect = false;
	int intersected_index = -1;
	for (int i = 0; i < units.entities.size(); i++)
	{
		// Skip weapon owner.
		if (units.entities.at(i).entity->getID() == weapon_component->mOwnerEntity)
		{
			continue;
		}
		ObjectCollisionComponent* p_current_collision = getComponentFromKnownEntity<ObjectCollisionComponent>(units.entities.at(i).entity->getID());
		TransformComponent* p_current_transform = getComponentFromKnownEntity<TransformComponent>(units.entities.at(i).entity->getID());

		// Grabbing copy of AABB from current and transforming to world space.
		AABB current_aabb = p_current_collision->mAABB;
		XMMATRIX current_world_transform = UtilityEcsFunctions::GetWorldMatrix(*p_current_transform);
		current_aabb.BoundingBox::Transform(current_aabb, current_world_transform);		

		// Checking intersection and breaks if intersection.
		intersect = weapon_bv->Intersects(&current_aabb);
		if (intersect)
		{
			intersected_index = i;
			break;
		}
	}
	
	// if collision: 
// *check movement diff on weapon
// *calc velocity
// *multiply velocity with base damage for true damage
// *deduct damage from collided entities health

	weapon_component->mPreviousPos = weapon_transform_component->position;
}