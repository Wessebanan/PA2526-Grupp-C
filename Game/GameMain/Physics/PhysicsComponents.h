#pragma once
#include "ecs.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include <DirectXCollision.h>
#include "BoundingVolume.h"
#include "../GameGlobals.h"

#define COMP(name) struct name : public ecs::ECSComponent<name>

// A bunch of default values.
constexpr float DEFAULT_MOVEMENT_FORCE	= 300.0f;
constexpr float DEFAULT_DECELERATION	= 200.0f;
constexpr float DEFAULT_MAX_VELOCITY	= 20.0f;
constexpr float DEFAULT_WEIGHT			= 250.0f;
constexpr float DEFAULT_GRAVITY			= 9.82f;
constexpr float DEFAULT_HEALTH			= 100.0f;

constexpr float BASE_SWORD_DAMAGE		= 10.0f;
constexpr float BASE_FIST_DAMAGE		= 2.0f;

// Base knockback is a force in newtons while weapon
// specific knockbacks are multipliers.
constexpr float BASE_KNOCKBACK			= 50.0f;
constexpr float SWORD_KNOCKBACK			= 2.0f;
constexpr float FIST_KNOCKBACK			= 0.5f;

constexpr float BASE_INVINCIBILITY_TIME = 0.2f;

constexpr double PI = 3.14159265358979323846;

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
		};

		/*
		* BoundingSphereComponent holds a description
		* of a bounding sphere, which is necessary
		* to calculate collision. Any entity that
		* should check collision needs this.
		*/
		COMP(BoundingSphereComponent)
		{
			DirectX::XMFLOAT3 mCenter;
			float mRadius;
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
			AABB mAABB;

			Sphere *mSpheres = nullptr;
			unsigned int mSphereCount = 0;

			// States if the last movement resulted in collision
			// and needs to be reverted.
			bool mIntersect = false;

			~ObjectCollisionComponent()
			{
				if (mSpheres)
				{
					delete[] mSpheres;
				}
			}
		};

		/*
		* Temporary mesh component to test with ECS.
		*/
		COMP(MeshComponent)
		{
			ModelLoader::Mesh *mMesh;
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
		};

		/*
		* Holds an invincibility timer for units.
		*/
		COMP(InvincilibityTimer)
		{
			float mTime;
		};
	} // components
} // ecs

