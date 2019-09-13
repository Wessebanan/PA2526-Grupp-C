#pragma once
#include "ecs.h"
#include <DirectXMath.h>
#define COMP(name) struct name : public ecs::ECSComponent<name>

/*
* MovementComponent holds velocity
* and direction which are needed to
* change the position of an entity.
*/
COMP(MovementComponent)
{
	float mVelocity;
	DirectX::XMFLOAT3 mDirection;
};

/*
* HitboxComponent holds a description
* of a hitbox, which is necessary
* to calculate collision.
*/
COMP(HitboxComponent)
{
	DirectX::XMFLOAT3 mOrigin;
	float mHeight;
	float mWidth;
	float mLength;
};

/*
* ForceComponent holds the center of mass
* and weight of an entity to calculate
* the acceleration of said entity.
*/
COMP(ForceComponent)
{
	DirectX::XMFLOAT3 mMassCenter;
	float mWeight;
};
