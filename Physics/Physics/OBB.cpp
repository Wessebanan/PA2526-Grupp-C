#include "OBB.h"

OBB::OBB()
{
	mpVertices = new XMFLOAT3[8];
}

OBB::~OBB()
{
	delete[] mpVertices;
}

void OBB::Generate(const XMFLOAT3* points, const unsigned int size)
{
	XMFLOAT3 min, max;
	GetExtremes(points, size, min, max);

	// Creating the vertices for the box with the min and max points.

	// Near z:	2 3
	//			0 1
	mpVertices[0] = min;
	mpVertices[1] = DirectX::XMFLOAT3(max.x, min.y, min.z);
	mpVertices[2] = DirectX::XMFLOAT3(min.x, max.y, min.z);
	mpVertices[3] = DirectX::XMFLOAT3(max.x, max.y, min.z);

	// Far z:	6 7
	//			4 5
	mpVertices[4] = DirectX::XMFLOAT3(min.x, min.y, max.z);
	mpVertices[5] = DirectX::XMFLOAT3(max.x, min.y, max.z);
	mpVertices[6] = DirectX::XMFLOAT3(min.x, max.y, max.z);
	mpVertices[7] = max;
}

bool OBB::Intersect(const BoundingVolume* volume)
{
	const OBB* obb = dynamic_cast<const OBB*>(volume);
	if (obb)
	{
		return IntersectOBB(volume);
	}
	const AABB* aabb = dynamic_cast<const AABB*>(volume);
	if (aabb)
	{
		return IntersectAABB(volume);
	}
	const BoundingSphere* sphere = dynamic_cast<const BoundingSphere*>(volume);
	if (sphere)
	{
		return IntersectSphere(volume);
	}
}

void OBB::WorldTransform(const XMMATRIX& world)
{
}

const XMFLOAT3* OBB::GetVertices() const
{
	return mpVertices;
}

const XMFLOAT3* OBB::GetCenter() const
{
	return &mCenter;
}

bool OBB::IntersectSphere(const BoundingVolume* volume)
{
	return false;
}

bool OBB::IntersectOBB(const BoundingVolume* volume)
{
	const OBB* obb = dynamic_cast<const OBB*>(volume);



	return false;
}

bool OBB::IntersectAABB(const BoundingVolume* volume)
{
	return false;
}
