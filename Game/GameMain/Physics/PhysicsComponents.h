#pragma once
#include "ecs.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include <DirectXCollision.h>

#define COMP(name) struct name : public ecs::ECSComponent<name>

// A bunch of default values.
#define DEFAULT_MOVEMENT_FORCE 500.0f
#define DEFAULT_DECELERATION 10.0f
#define DEFAULT_MAX_VELOCITY 100.0f
#define DEFAULT_WEIGHT 50.0f
#define DEFAULT_GRAVITY 9.82f
#define DEFAULT_HEALTH 100.0f
#define DEFAULT_BASE_DAMAGE 10.0f

// WEAPON_TYPE decides what bounding volume to use.
enum WEAPON_TYPE
{
	SWORD,
	PROJECTILE,
	DEFAULT
};

// Inheritance structure for unspecific bounding volumes.
#pragma region BoundingVolume
struct BoundingVolume 
{
	virtual ~BoundingVolume() {}
};
struct Sphere : public BoundingVolume
{
	DirectX::BoundingSphere mSphere;
};
struct OBB : public BoundingVolume
{
	DirectX::BoundingOrientedBox mOBB;
};
struct AABB : public BoundingVolume
{
	DirectX::BoundingBox mAABB;
};
#pragma endregion

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
			DirectX::XMFLOAT3 mAcceleration = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mVelocity		= DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mForward		= DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);

			// Assuming objects are equal in
			// density all over.
			DirectX::XMFLOAT3 mMassCenter;

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
			DirectX::BoundingOrientedBox mOBB;

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
			DirectX::BoundingBox mAABB;

			DirectX::BoundingSphere *mSpheres = nullptr;
			unsigned int mSphereCount = 0;

			// States if the last movement resulted in collision
			// and needs to be reverted.
			bool mIntersect = false;

			~ObjectCollisionComponent()
			{
				delete[] mSpheres;
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
			WEAPON_TYPE mType = DEFAULT;

			BoundingVolume* mBoundingVolume = nullptr;

			// Previous position to calculate velocity for damage.
			DirectX::XMFLOAT3 mPreviousPos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
			
			// Base damage for multiplier on hit based on weapon type.
			float mBaseDamage = DEFAULT_BASE_DAMAGE;
		};

		/*
		* Holds health at the moment (HealthComponent is already defined)
		* might hold strength, stamina, speed etc. in the future so constitution
		* is a good name.
		*/
		COMP(ConstitutionComponent)
		{
			float mBaseHealth	= DEFAULT_HEALTH;
			float mHealth		= DEFAULT_HEALTH;
		};
	} // components
} // ecs

