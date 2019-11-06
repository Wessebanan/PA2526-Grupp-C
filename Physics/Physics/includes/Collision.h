#pragma once
#include "CollisionObject.h"

class Collision
{
public:
	DWORD mNumObjects;
	CollisionObject* mObjects;

	Collision();
	~Collision();

	void Free();
	void AddSphere(DirectX::XMFLOAT3 pVecPos, float radius);
	void AddPlane(DirectX::XMFLOAT4* pPlaneParams);
};