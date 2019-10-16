#include "FightingSystem.h"
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
