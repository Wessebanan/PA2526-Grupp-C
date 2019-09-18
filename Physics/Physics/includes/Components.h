#pragma once
#include "ecs.h"
#include <DirectXMath.h>
#define COMP(name) struct name : public ecs::ECSComponent<name>

namespace ecs
{
	namespace components
	{
		/*
		* MovementComponent holds velocity
		* and direction which are needed to
		* change the position of an entity.
		*/
		COMP(MovementComponent)
		{
			float mMaxVelocity = 100.0f;
			float mVelocity = 0.0f;
			DirectX::XMFLOAT3 mForward = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
			DirectX::XMFLOAT3 mDirection = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);	
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

	} // components
} // ecs

