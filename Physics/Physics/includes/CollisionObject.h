#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

class CollisionObject
{
public:
	DWORD mType;
	//DirectX::XMFLOAT3 mVecPos;
	//float mRadius;
	//DirectX::XMFLOAT4 mPlane;
	BoundingVolume* mBoundingVolume;
	CollisionObject* mNext;

	CollisionObject() { mNext = nullptr; };
	~CollisionObject() { delete mNext; mNext = nullptr; };
};