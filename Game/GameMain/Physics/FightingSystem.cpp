#include "FightingSystem.h"
#include "GridProp.h"
#include "GridFunctions.h"
#include "../MeshContainer/MeshContainer.h"

#include "../gameGraphics/ParticleECSComponents.h"

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

	TransformComponent* transform_component = getComponentFromKnownEntity<TransformComponent>(create_component_event->entityID);
	
	// Need transform component for scaling attack range.
	if (!transform_component)
	{
		return;
	}
	
	WeaponComponent* weapon_component = getComponent<WeaponComponent>(create_component_event->componentID);
	std::vector<XMFLOAT3>* vertices = nullptr;

	switch (weapon_component->mType)
	{
	// DirectXCollision is aggressive as f when making OBBs so I make
	// an AABB and then an OBB out of it to avoid a bunch of points being
	// outside of the OBB.
	case GAME_OBJECT_TYPE_WEAPON_SWORD:
	{
		vertices = MeshContainer::GetMeshCPU(weapon_component->mType)->GetVertexPositionVector();
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


	case GAME_OBJECT_TYPE_WEAPON_HAMMER:
	{
		vertices = MeshContainer::GetMeshCPU(weapon_component->mType)->GetVertexPositionVector();
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

		weapon_component->mBaseDamage = BASE_HAMMER_DAMAGE;

		weapon_component->mKnockback = HAMMER_KNOCKBACK;
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
	case GAME_OBJECT_TYPE_WEAPON_BOMB:
	{
		// Fist is only a unit sphere, supposed to get hand transform.
		weapon_component->mBoundingVolume = new Sphere;
		Sphere* sphere = static_cast<Sphere*>(weapon_component->mBoundingVolume);
		sphere->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
		sphere->Radius = BOMB_PICKUP_RADIUS;

		// TODO: Get arm length and set to attack range.
		weapon_component->mWeaponRange = BOMB_ATTACK_RANGE;

		weapon_component->mBaseDamage = BASE_BOMB_DAMAGE;

		weapon_component->mKnockback = BOMB_KNOCKBACK;
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
	
	// If the owner unit does not have an attack state component, return.
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
	case GAME_OBJECT_TYPE_WEAPON_HAMMER:
	{
		OBB* obb = static_cast<OBB*>(weapon_component->mBoundingVolume);
		weapon_bv = new OBB(*obb);
		break;
	}
	case GAME_OBJECT_TYPE_WEAPON_FIST:
	case GAME_OBJECT_TYPE_WEAPON_BOMB:
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

		// Don't do anything with a unit that is invincible.
		if (units.entities.at(i).entity->hasComponentOfType<InvincilibityTimer>())
		{
			continue;
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
	if (unit_entity == nullptr && intersect)
	{
		EquipmentComponent *equipment_component = getComponentFromKnownEntity<EquipmentComponent>(collided_unit);
		if (equipment_component->mEquippedWeapon != 0)
		{
			WeaponComponent* current_weapon = getComponentFromKnownEntity<WeaponComponent>(equipment_component->mEquippedWeapon);
			// If it's the same weapon type, don't do anything.
			if (!current_weapon || current_weapon->mType == weapon_component->mType)
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
			sound.audioName = AudioName::SOUND_get_item;
			sound.soundFlags = SF_NONE;
			sound.invokerEntityId = 0;
			createEvent(sound);
		}

		equipment_component->mAttackRange = equipment_component->mMeleeRange + weapon_component->mWeaponRange;

		if (weapon_component->mType == GAME_OBJECT_TYPE_WEAPON_BOMB)
		{
			static_cast<Sphere*>(weapon_component->mBoundingVolume)->Radius = TO_UNIT_SCALE(BOMB_ATTACK_RANGE);
		}

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
	// If weapon is a bomb, create "OnHitEvent" with a bomb
	else if (intersect && weapon_component->mType == GAME_OBJECT_TYPE_WEAPON_BOMB)
	{
		WeaponOnHitEvent hit_event;
		hit_event.Type			= weapon_component->mType;
		hit_event.Position		= weapon_transform_component->position;
		hit_event.Range			= BOMB_BLAST_RADIUS;
		hit_event.Damage		= weapon_component->mBaseDamage;
		hit_event.Knockback		= weapon_component->mKnockback;
		hit_event.WeaponID		= weapon_component->getEntityID();
		hit_event.OwnerUnitID	= weapon_component->mOwnerEntity;

		createEvent(hit_event);
	}
	else if (intersect)
	{
		// DAMAGE

		// Equipment component used for damage multiplier
		EquipmentComponent* equipment_component = getComponentFromKnownEntity<EquipmentComponent>(unit_entity->getID());
		// Calculating velocity on weapon.
		float movement = CalculateDistance(weapon_component->mPreviousPos, weapon_bv->GetCenter());
		float velocity = movement / _delta;

		// Capping velocity to not get insane velocity when units rotate the same frame.
		velocity = (std::min)(5.0f, velocity);

		// Calculating damage by multiplying weapon velocity and the base damage.
		float damage = velocity * weapon_component->mBaseDamage * equipment_component->mAttackMultiplier;

		HealthComponent *collided_constitution = getComponentFromKnownEntity<HealthComponent>(collided_unit);

		collided_constitution->mHealth -= damage;
		collided_constitution->mHitBy = unit_entity->getID();
		
		// INVINCIBILITY
		// (based on damage dealt)
		InvincilibityTimer timer;
		timer.mTime = log2f(damage) * BASE_INVINCIBILITY_TIME;
		createComponent<InvincilibityTimer>(collided_unit, timer);

		// KNOCKBACK
		ForceImpulseEvent knockback;
		knockback.mDirection = getComponentFromKnownEntity<DynamicMovementComponent>(unit_entity->getID())->mDirection;

		// Small y boost in knockback to send units FLYING.
		knockback.mDirection.y += 0.3f;

		// Normalize knockback direction so it's not CRAZY.
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
			death_sound_event.soundFlags = SF_RANDOM_PITCH;
			death_sound_event.audioName = AudioName::SOUND_scream;
			death_sound_event.invokerEntityId = collided_unit;
			createEvent(death_sound_event); // Play death sound

			// If we only want to give kill reward on killing blows put that code here

		}
		else
		{
			ecs::events::PlaySound damage_sound_event;
			damage_sound_event.soundFlags = SF_RANDOM_PITCH;
			float choose_hurt_sound = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			if(choose_hurt_sound <= 0.85f)
				damage_sound_event.audioName = AudioName::SOUND_grunt1;
			else
				damage_sound_event.audioName = AudioName::SOUND_grunt2;
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
	updateType = EntityUpdate;
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
	
	Color color = p_color_switch->mColor;
	p_unit_color->red	= color.r;
	p_unit_color->green = color.g;
	p_unit_color->blue	= color.b;

	// Inserting given time by event into [ID] in unordered_map.
	mTimers[p_color_switch->mEntityID] = p_color_switch->mTime;
}
void ecs::systems::UnitColorSwitchSystem::updateEntity(FilteredEntity& _entity, float _delta)
{
		ID current = _entity.entity->getID();

		// Checking to see if the current entity has a color switch timer.
		auto timer = mTimers.find(current);
		if (timer != mTimers.end())
		{
			// Decreasing the timer by delta and erasing if time's up.
			timer->second -= _delta;
			if (timer->second < 0.0f)
			{
				mTimers.erase(timer->first);
			}
			// Return if unit has a timer.
			return;
		}

		UnitComponent* p_unit = getComponentFromKnownEntity<UnitComponent>(current);
		ColorComponent* p_color = getComponentFromKnownEntity<ColorComponent>(current);
		HealthComponent* p_health = getComponentFromKnownEntity<HealthComponent>(current);

		Color color = Color(0, 0, 0);
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

		// Finding what percentage of health remains for color calculation.
		float health_fraction = p_health->mHealth / p_health->mBaseHealth;
		
		// Applying health fraction to color channels to make units darker if damaged.
		p_color->red	= (uint8_t)((float)color.r * health_fraction);
		p_color->green	= (uint8_t)((float)color.g * health_fraction);
		p_color->blue	= (uint8_t)((float)color.b * health_fraction);
}
#pragma endregion
#pragma region UnitInvincibilityTimerSystem
ecs::systems::UnitInvincibilityTimerSystem::UnitInvincibilityTimerSystem()
{
	updateType = EntityUpdate;
	typeFilter.addRequirement(InvincilibityTimer::typeID);
}

ecs::systems::UnitInvincibilityTimerSystem::~UnitInvincibilityTimerSystem()
{

}

void ecs::systems::UnitInvincibilityTimerSystem::updateEntity(FilteredEntity& _entity, float _delta)
{
	InvincilibityTimer* timer = getComponentFromKnownEntity<InvincilibityTimer>(_entity.entity->getID());
	timer->mTime -= _delta;
	if (timer->mTime < 0.0f)
	{
		removeComponent(_entity.entity->getID(), InvincilibityTimer::typeID);
	}
}
#pragma endregion

#pragma region WeaponOnHitSystem

ecs::systems::WeaponOnHitSystem::WeaponOnHitSystem()
{
	updateType = EventReader;
	typeFilter.addRequirement(WeaponOnHitEvent::typeID);
}

ecs::systems::WeaponOnHitSystem::~WeaponOnHitSystem()
{

}

void ecs::systems::WeaponOnHitSystem::readEvent(BaseEvent& _event, float _delta)
{
	if (_event.getTypeID() != WeaponOnHitEvent::typeID)
	{
		return;
	}

	const WeaponOnHitEvent& hit_event = static_cast<WeaponOnHitEvent&>(_event);
	const XMVECTOR weapon_position = XMLoadFloat3(&hit_event.Position);

	// Grabbing and storing all ocean tiles.
	TypeFilter unit_filter;
	unit_filter.addRequirement(components::UnitComponent::typeID);
	unit_filter.addRequirement(components::TransformComponent::typeID);
	unit_filter.addRequirement(components::HealthComponent::typeID);

	EntityIterator iter = getEntitiesByFilter(unit_filter);

	// If type is a bomb
	if (hit_event.Type == GAME_OBJECT_TYPE_WEAPON_BOMB)
	{
		for (FilteredEntity& unit : iter.entities)
		{
			const TransformComponent* p_unit_transform = unit.getComponent<TransformComponent>();
			const XMVECTOR unit_position = XMLoadFloat3(&p_unit_transform->position);
			const XMVECTOR unit_weapon_v = unit_position - weapon_position;
			const float unit_weapon_dist = XMVectorGetX(XMVector3Length(unit_weapon_v));
			
			/*
				Impact:
					 = 1.0f : At Bomb
					>= 0.0f : Within Range
					<  0.0f : Not within range

				Can be used to decrease dmg and knockback with distance
			*/
			const float impact = (hit_event.Range - unit_weapon_dist) / hit_event.Range;


			// if unit will receive impact (Looks at all units)
			if (impact >= 0.0f)
			{
				// Calculating damage by multiplying weapon velocity and the base damage.
				const float damage = hit_event.Damage;
				HealthComponent* p_collided_constitution = unit.getComponent<HealthComponent>();
				p_collided_constitution->mHealth -= damage;

				// KNOCKBACK
				ForceImpulseEvent knockback;
				XMStoreFloat3(&knockback.mDirection, unit_weapon_v);
				XMStoreFloat3(&knockback.mDirection, XMVector3Normalize(XMLoadFloat3(&knockback.mDirection)));

				// Small y boost in knockback to send units FLYING.
				knockback.mDirection.y += 1.f;

				// Normalize knockback direction so it's not CRAZY.
				XMStoreFloat3(&knockback.mDirection, XMVector3Normalize(XMLoadFloat3(&knockback.mDirection)));

				knockback.mForce = hit_event.Knockback;
				knockback.mEntityID = unit.entity->getID();
				createEvent(knockback);


				// VISUAL
				ColorSwitchEvent damage_flash;
				damage_flash.mColor = WHITE;
				damage_flash.mEntityID = unit.entity->getID();
				damage_flash.mTime = 0.05f;
				createEvent(damage_flash);
			}
		}

		// Make Explosion!
		ecs::components::ParticleSpawnerComponent particle_spawner;
		ecs::components::BombSpawnerComponent bomb_spawner;

		particle_spawner.StartPosition = hit_event.Position;
		particle_spawner.SpawnFrequency = 0.1f;
		particle_spawner.LifeDuration = 0.5f;

		bomb_spawner.InitialVelocity = 16.0f;
		bomb_spawner.SpawnCount = 2000.0f;

		createEntity(particle_spawner, bomb_spawner);

		// Sound
		events::PlaySound m_event;
		m_event.audioName = AudioName::SOUND_kaboom;
		createEvent(m_event);

		// ---

		components::EquipmentComponent* p_equipment = getComponentFromKnownEntity<components::EquipmentComponent>(hit_event.OwnerUnitID);

		// Remove bomb after use
		if (p_equipment)
		{

			removeEntity(hit_event.WeaponID);

			/*
				Create fist entity
			*/

			WeaponComponent fist_weapon;
			TransformComponent fist_transform;
			fist_weapon.mType = GAME_OBJECT_TYPE_WEAPON_FIST;
			fist_weapon.mOwnerEntity = hit_event.OwnerUnitID;
			fist_transform.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
			Entity* p_fist = createEntity(fist_transform, fist_weapon);

			p_equipment->mEquippedWeapon = p_fist->getID();
			p_equipment->mAttackRange = p_equipment->mMeleeRange;
		}
	}
}

#pragma endregion
