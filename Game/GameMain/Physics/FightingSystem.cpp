#include "FightingSystem.h"
#include "GridProp.h"
#include "GridFunctions.h"

#pragma region WeaponInitSystem
ecs::systems::WeaponInitSystem::WeaponInitSystem()
{
	updateType = EventListenerOnly;
	subscribeEventCreation(CreateComponentEvent::typeID);
}
ecs::systems::WeaponInitSystem::~WeaponInitSystem()
{
	ComponentIterator it = getComponentsOfType<WeaponComponent>();
	while (WeaponComponent * weapon = static_cast<WeaponComponent*>(it.next()))
	{
		delete weapon->mBoundingVolume;
	}
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
	if (weapon_component->mType != GAME_OBJECT_TYPE_WEAPON_FIST)
	{
		vertices = mesh_component->mMesh->GetVertexPositionVector();
	}

	switch (weapon_component->mType)
	{
	// DirectXCollision is aggressive as f when making OBBs so I make
	// an AABB and then an OBB out of it to avoid a bunch of points being
	// outside of the OBB.
	case GAME_OBJECT_TYPE_WEAPON_SWORD:
	{
		weapon_component->mBoundingVolume = new OBB;
		OBB* obb = static_cast<OBB*>(weapon_component->mBoundingVolume);
		obb->CreateFromPoints(*(BoundingOrientedBox*)obb, vertices->size(), vertices->data(), sizeof(XMFLOAT3));

		OBB temp_obb = OBB(*obb);
		// Applying scale to aabb to find actual range.
		temp_obb.Transform(UtilityEcsFunctions::GetWorldMatrix(*transform_component));

		// Finding greatest extent in obb and setting that (*2) to attack range (for now).
		XMFLOAT3 extents = temp_obb.Extents;
		weapon_component->mWeaponRange = extents.x > extents.y ? (extents.x > extents.z ? extents.x : extents.z) : (extents.y > extents.z ? extents.y : extents.z);
		weapon_component->mWeaponRange *= 2;

		weapon_component->mBaseDamage = BASE_SWORD_DAMAGE;

		weapon_component->mKnockback = SWORD_KNOCKBACK;
		break;
	}

	case GAME_OBJECT_TYPE_WEAPON_FIST:
	{
		// Fist is only a unit sphere, supposed to get hand transform.
		weapon_component->mBoundingVolume = new Sphere;
		Sphere* sphere = static_cast<Sphere*>(weapon_component->mBoundingVolume);
		sphere->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
		sphere->Radius = 3.0f;

		// TODO: Get arm length and set to attack range.
		weapon_component->mWeaponRange = 0.0f;

		weapon_component->mBaseDamage = BASE_FIST_DAMAGE;

		weapon_component->mKnockback = FIST_KNOCKBACK;
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

	if (weapon->getComponentCount() == 0)
	{
		return;
	}

	WeaponComponent* weapon_component = getComponentFromKnownEntity<WeaponComponent>(weapon->getID());
	TransformComponent* weapon_transform_component = getComponentFromKnownEntity<TransformComponent>(weapon->getID());
	Entity* unit_entity = ECSUser::getEntity(weapon_component->mOwnerEntity);
	if (unit_entity != nullptr)
	{
		if (!unit_entity->hasComponentOfType(AttackStateComponent::typeID))
		{
			return;
		}
	}
	

	// FILL OUT WITH OTHER WEAPONS LATER
	// Making a copy of the bounding volume for weapon.
	BoundingVolume* weapon_bv = nullptr;
	switch (weapon_component->mType)
	{
	case GAME_OBJECT_TYPE_WEAPON_SWORD:
	{
		OBB* obb = static_cast<OBB*>(weapon_component->mBoundingVolume);
		weapon_bv = new OBB(*obb);
		break;
	}
	case GAME_OBJECT_TYPE_WEAPON_FIST:
	{
		Sphere* sphere = static_cast<Sphere*>(weapon_component->mBoundingVolume);
		weapon_bv = new Sphere(*sphere);
	}
	default:
		break;
	}

	// Army component for checking friendly fire.
	UnitComponent* owner_unit_component = nullptr;

	// Transforming weapon bv to world space if no owner.
	if (weapon_component->mOwnerEntity == 0 || unit_entity == nullptr)
	{
		XMMATRIX weapon_world = UtilityEcsFunctions::GetWorldMatrix(*weapon_transform_component);
		weapon_bv->Transform(weapon_world);
	}

	// Transform weapon to weapon owner right hand position if it has owner, also get army unit IDs.
	else
	{	
		SkeletonComponent* p_skeleton = getComponentFromKnownEntity<SkeletonComponent>(weapon_component->mOwnerEntity);
		XMFLOAT4X4 right_hand_offset_matrix = p_skeleton->skeletonData.GetOffsetMatrixUsingJointName("Hand.r");	
		TransformComponent* p_owner_transform = getComponentFromKnownEntity<TransformComponent>(weapon_component->mOwnerEntity);

		// Assigning unit transform component to weapon transform component for now.
		weapon_transform_component->scale		= p_owner_transform->scale;
		weapon_transform_component->position	= p_owner_transform->position;
		weapon_transform_component->rotation	= p_owner_transform->rotation;

		// Hand position in model space.
		XMFLOAT3 origin_to_hand = ORIGIN_TO_HAND;
		XMMATRIX hand_trans = XMMatrixTranslationFromVector(XMLoadFloat3(&origin_to_hand));

		// Final world transform.
		XMMATRIX world = hand_trans * XMMatrixTranspose(XMLoadFloat4x4(&right_hand_offset_matrix)) * UtilityEcsFunctions::GetWorldMatrix(*p_owner_transform);

		weapon_bv->Transform(world);

		owner_unit_component = getComponentFromKnownEntity<UnitComponent>(weapon_component->mOwnerEntity);
	}

	// Grab all entities with a health component (units).
	TypeFilter filter;
	filter.addRequirement(HealthComponent::typeID);
	filter.addRequirement(ObjectCollisionComponent::typeID);
	filter.addRequirement(TransformComponent::typeID);
	EntityIterator units = getEntitiesByFilter(filter);

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

		// Checking for friendly fire.
		if (owner_unit_component)
		{
			if (owner_unit_component->playerID == getComponentFromKnownEntity<UnitComponent>(current_unit)->playerID)
			{
				continue;
			}
		}

		ObjectCollisionComponent* p_current_collision = getComponentFromKnownEntity<ObjectCollisionComponent>(current_unit);
		TransformComponent* p_current_transform = getComponentFromKnownEntity<TransformComponent>(current_unit);

		// Grabbing copy of AABB from current and transforming to world space.
		AABB current_aabb = p_current_collision->mAABB;
		XMMATRIX current_world_transform = UtilityEcsFunctions::GetWorldMatrix(*p_current_transform);
		current_aabb.Transform(current_world_transform);		

		// Checking intersection and breaks if intersection.
		intersect = weapon_bv->Intersects(&current_aabb);
		if (intersect)
		{
			collided_unit = current_unit;
			break;
		}
	}

	// If a unit collides with an unowned weapon, set colliding unit to weapon owner
	// and colliding unit equipment to weapon.
	if (weapon_component->mOwnerEntity == 0 && intersect)
	{
		EquipmentComponent *equipment_component = getComponentFromKnownEntity<EquipmentComponent>(collided_unit);
		if (equipment_component->mEquippedWeapon != 0)
		{
			WeaponComponent* current_weapon = getComponentFromKnownEntity<WeaponComponent>(equipment_component->mEquippedWeapon);
			// If it's the same weapon type, don't do anything.
			if (current_weapon->mType == weapon_component->mType)
			{
				return;
			}
			// Remove current weapon.
			removeEntity(equipment_component->mEquippedWeapon);
		}	

		///////////////////////////////////////////////
		///////////////SOUND HERE//////////////////////
		///////////////////////////////////////////////

		{
			ecs::events::PlaySound sound;
			sound.audioName = AudioName::ITEM_GET_SOUND;
			sound.soundFlags = SF_NONE;
			sound.invokerEntityId = 0;
			createEvent(sound);
		}

		equipment_component->mAttackRange = equipment_component->mMeleeRange + weapon_component->mWeaponRange;

		equipment_component->mEquippedWeapon = weapon->getID();
		weapon_component->mOwnerEntity = collided_unit;
		GridProp* p_gp = GridProp::GetInstance();
		int2 tile_index = GridFunctions::GetTileFromWorldPos(weapon_transform_component->position.x, weapon_transform_component->position.z);
		unsigned int tile_id = p_gp->mGrid[tile_index.y][tile_index.x].Id;
		for (int l = 0; l < p_gp->mLootTiles.size(); l++)
		{
			if (tile_id == p_gp->mLootTiles[l])
			{
				p_gp->mLootTiles.erase(p_gp->mLootTiles.begin() + l);
				break;
			}
		}
	}
	else if (intersect)
	{
		// DAMAGE

		// Calculating velocity on weapon.
		float movement = CalculateDistance(weapon_component->mPreviousPos, weapon_bv->GetCenter());
		float velocity = movement / _delta;

		// Capping velocity to not get insane velocity when units rotate the same frame.
		velocity = (std::min)(2.0f, velocity);

		// Calculating damage by multiplying weapon velocity and the base damage.
		float damage = velocity * weapon_component->mBaseDamage;

		HealthComponent *collided_constitution = getComponentFromKnownEntity<HealthComponent>(collided_unit);

		collided_constitution->mHealth -= damage;
		
		// KNOCKBACK
		ForceImpulseEvent knockback;
		knockback.mDirection = getComponentFromKnownEntity<DynamicMovementComponent>(unit_entity->getID())->mDirection;
		knockback.mDirection.y += 0.3f;

		XMStoreFloat3(&knockback.mDirection, XMVector3Normalize(XMLoadFloat3(&knockback.mDirection)));
		
		knockback.mForce = BASE_KNOCKBACK * velocity * weapon_component->mKnockback;
		knockback.mEntityID = collided_unit;
		createEvent(knockback);

		// SOUND
		if (collided_constitution->mHealth <= 0.0f && !ECSUser::getEntity(collided_unit)->hasComponentOfType(DeadComponent::typeID))
		{
			ecs::components::DeadComponent dead_comp;
			ecs::ECSUser::createComponent(collided_constitution->getEntityID(), dead_comp);
			ecs::events::PlaySound death_sound_event;
			death_sound_event.soundFlags = SF_NONE;
			death_sound_event.audioName = AudioName::SCREAM_SOUND;
			death_sound_event.invokerEntityId = collided_unit;
			createEvent(death_sound_event); // Play death sound
		}
		else
		{
			ecs::events::PlaySound damage_sound_event;
			damage_sound_event.soundFlags = SF_NONE;
			float choose_hurt_sound = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			if(choose_hurt_sound <= 0.4999999f)
				damage_sound_event.audioName = AudioName::GRUNT_HURT_1_SOUND;
			else
				damage_sound_event.audioName = AudioName::GRUNT_HURT_2_SOUND;
			damage_sound_event.invokerEntityId = collided_unit;
			createEvent(damage_sound_event); // Play damage sound
		}

		// VISUAL
		ColorSwitchEvent damage_flash;
		damage_flash.mColor = WHITE;
		damage_flash.mEntityID = collided_unit;
		damage_flash.mTime = 0.05f;
		createEvent(damage_flash);
	}
	
	weapon_component->mPreviousPos = weapon_bv->GetCenter();
	delete weapon_bv;
}
#pragma endregion
#pragma region UnitColorSwitchSystem
ecs::systems::UnitColorSwitchSystem::UnitColorSwitchSystem()
{
	updateType = MultiEntityUpdate;
	typeFilter.addRequirement(UnitComponent::typeID);
	typeFilter.addRequirement(ColorComponent::typeID);
	typeFilter.addRequirement(HealthComponent::typeID);

	subscribeEventCreation(ColorSwitchEvent::typeID);
}
ecs::systems::UnitColorSwitchSystem::~UnitColorSwitchSystem()
{

}
void ecs::systems::UnitColorSwitchSystem::onEvent(TypeID _typeID, ecs::BaseEvent* _event)
{
	ColorSwitchEvent* p_color_switch = static_cast<ColorSwitchEvent*>(_event);
	
	ColorComponent* p_unit_color = getComponentFromKnownEntity<ColorComponent>(p_color_switch->mEntityID);
	
	uint3 color = p_color_switch->mColor;
	p_unit_color->red	= color.r;
	p_unit_color->green = color.g;
	p_unit_color->blue	= color.b;

	mTimers[p_color_switch->mEntityID] = p_color_switch->mTime;
}
void ecs::systems::UnitColorSwitchSystem::updateMultipleEntities(EntityIterator& _entities, float _delta)
{
	for (int i = 0; i < _entities.entities.size(); i++)
	{
		ID current = _entities.entities.at(i).entity->getID();
		auto timer = mTimers.find(current);
		if (timer != mTimers.end())
		{
			timer->second -= _delta;
			if (timer->second < 0.0f)
			{
				mTimers.erase(timer->first);
			}
			continue;
		}

		UnitComponent* p_unit = getComponentFromKnownEntity<UnitComponent>(current);
		ColorComponent* p_color = getComponentFromKnownEntity<ColorComponent>(current);
		HealthComponent* p_health = getComponentFromKnownEntity<HealthComponent>(current);

		float health_fraction = p_health->mHealth / p_health->mBaseHealth;
		uint3 color = uint3(0, 0, 0);
		switch (p_unit->playerID)
		{
		case PLAYER1:		
			color = PLAYER1_COLOR;
			break;		
		case PLAYER2:
			color = PLAYER2_COLOR;
			break;
		case PLAYER3:
			color = PLAYER3_COLOR;
			break;
		case PLAYER4:
			color = PLAYER4_COLOR;
			break;
		}
		p_color->red	= (uint8_t)((float)color.r * health_fraction);
		p_color->green	= (uint8_t)((float)color.g * health_fraction);
		p_color->blue	= (uint8_t)((float)color.b * health_fraction);
	}
}
#pragma endregion