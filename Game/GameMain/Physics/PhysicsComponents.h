#pragma once
#include "ecs.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include <DirectXCollision.h>
#include "BoundingVolume.h"
#include "../GameGlobals.h"
#include "..//MeshContainer/MeshContainer.h"

#define COMP(name) struct name : public ecs::ECSComponent<name>


/*
	TO_UNIT_SCALE (x)
	
	Unit applies 0.1 scale to weapon's bounding volume to fit in hand

	'x' is defined in world space but will be projected onto unit scale
	Does ONLY apply to weapon when hitting other units
*/
#define TO_UNIT_SCALE(x) (x / 0.1f)  

// A bunch of default values.
constexpr float DEFAULT_MOVEMENT_FORCE	= 150.0f;
constexpr float DEFAULT_DECELERATION	= 25.0f;
constexpr float DEFAULT_MAX_VELOCITY	= 3.0f;
constexpr float DEFAULT_WEIGHT			= 50.0f;
constexpr float DEFAULT_GRAVITY			= 9.82f;
constexpr float DEFAULT_HEALTH			= 100.0f;

constexpr float BASE_SWORD_DAMAGE		= 6.0f;
constexpr float BASE_HAMMER_DAMAGE		= 2.0f;
constexpr float BASE_FIST_DAMAGE		= 1.0f;
constexpr float BASE_BOMB_DAMAGE		= 30.0f;

// Base knockback is a force in newtons while weapon
// specific knockbacks are multipliers.
constexpr float BASE_KNOCKBACK			= 50.0f;

constexpr float HAMMER_KNOCKBACK_MULTIPLIER		= 2.0f;
constexpr float SWORD_KNOCKBACK_MULTIPLIER		= 1.0f;
constexpr float FIST_KNOCKBACK_MULTIPLIER		= 0.5f;
constexpr float BOMB_KNOCKBACK_MULTIPLIER		= 15.0f;

/* 
	BOMB SPECIFIC CONSTANTS 
*/
constexpr float BOMB_ATTACK_RANGE	= 1.0f;	// Activation Range
constexpr float BOMB_BLAST_RADIUS	= 8.0f;	// Blast Radius
constexpr float BOMB_PICKUP_RADIUS	= 1.0f;	// Pick-up Radius

constexpr float BASE_INVINCIBILITY_TIME = 0.1f;

// Specifically hard-coded for current dude mesh lmao.
#define ORIGIN_TO_HAND XMFLOAT3(-4.99f, 2.55f, -0.329f)

namespace ecs
{
	namespace components
	{
		/*
		* StaticMovementComponent holds velocity
		* and direction which are needed to
		* change the position of an entity.
		*/
		COMP(StaticMovementComponent)
		{
			float mMaxVelocity = 100.0f;
			float mVelocity = 0.0f;
			DirectX::XMFLOAT3 mForward = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mDirection = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		};
		
		COMP(DynamicMovementComponent)
		{
			float mMaxVelocity		= DEFAULT_MAX_VELOCITY;
			float mMovementForce	= DEFAULT_MOVEMENT_FORCE;
			float mDeceleration		= DEFAULT_DECELERATION;
			float mWeight			= DEFAULT_WEIGHT;
			float mGravity			= DEFAULT_GRAVITY;

			DirectX::XMFLOAT3 mDirection	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mForce		= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mInputForce	= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mAcceleration = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mVelocity		= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mForward		= DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f);

			// Position pre-movement to determine if the object moved since last update.
			DirectX::XMFLOAT3 mPreviousPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

			// If object is on ground.
			bool mOnGround = false;

			float mLastTileY = -INFINITY;
		};

		/*
		* GroundCollisionComponent is an OBB which only checks
		* against the ground plane for collision. Not using lowest
		* point as objects may rotate which requires recalculation 
		* of the lowest point.
		*/
		COMP(GroundCollisionComponent)
		{
			OBB mOBB;

			// Storing last y values to avoid unneccesary checks.
			float mLastY = INFINITY;
			float mLastTileY = INFINITY;
		};
		
		/*
		* Object collision component is in its current state
		* a non-adaptive AABB, since there needs to be vertex
		* groups and animations to begin using BVHs and 
		* adaptive bounding volumes.
		*/
		COMP(ObjectCollisionComponent)
		{
			BoundingVolume * mBV		= nullptr;
			BV_TYPE mBvType				= COLLISION_ERROR;
			GAME_OBJECT_TYPE mObjType	= GAME_OBJECT_TYPE_MESH_ERROR;

			// States if the last movement resulted in collision
			// and needs to be reverted.
			bool mIntersect = false;

			~ObjectCollisionComponent()
			{
				if (mBV)
				{
					delete mBV;
				}
			}
		};

		/*
		* Holds weapon type (SWORD, PROJECTILE etc.), unspecified bounding volume,
		* previous position for damage calculation on impact and base damage specific
		* to a weapon.
		*/
		COMP(WeaponComponent)
		{
			// When an entity gets the weapon, give owner entity id to component.
			ID mOwnerEntity = 0;			

			GAME_OBJECT_TYPE mType = GAME_OBJECT_TYPE_WEAPON_FIST;
			BoundingVolume* mBoundingVolume = nullptr;
			
			// Previous position to calculate velocity for damage.
			DirectX::XMFLOAT3 mPreviousPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			
			// Base damage for multiplier on hit based on weapon type.
			float mBaseDamage	= 0.0f;
			
			float mWeaponRange	= 0.0f;
			float mKnockback	= 1.0f;

			~WeaponComponent()
			{				
				if (mBoundingVolume)
				{
					delete mBoundingVolume;
				}
			}
		};

		/*
		* Holds health at the moment (HealthComponent is already defined)
		* might hold strength, stamina, speed etc. in the future so constitution
		* is a good name.
		*/
		COMP(HealthComponent)
		{
			float mBaseHealth	= DEFAULT_HEALTH;
			float mHealth		= DEFAULT_HEALTH;
			unsigned int mHitBy = 0;
		};

		COMP(QuadTreeComponent)
		{
			void* pTree;
		};

		/*
		* Holds the ID of the equipped weapon entity, as
		* well as the owner unit's melee range (arm length)
		* and the total attack range (melee + weapon range).
		*/
		COMP(EquipmentComponent)
		{
			ID mEquippedWeapon = 0;
			// Melee range is arm length.
			float mMeleeRange = 0.0f;

			// Attack range is sum of weapon range and melee range.
			float mAttackRange = 0.0f;

			// How much damage the units attacks does 1 is normal.
			float mAttackMultiplier = 1.0f;
		};

		/*
		* Holds an invincibility timer for units.
		*/
		COMP(InvincilibityTimer)
		{
			float mTime;
		};

		/*
		* Holds the tile ID and tile Y-pos that the weapon should fall to.
		*/
		COMP(FallingWeaponComponent)
		{
			//I'm important! Please don't remove me Baka Master-senpai!
			float mPosY;
			float mPosYOffset;
			ID mTileId;
			XMFLOAT3 rotation;
			ID mCarepackageId;
		};
	} // components
} // ecs

