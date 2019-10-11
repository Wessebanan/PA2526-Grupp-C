//#include "AABB.h"
//
//AABB::AABB()
//{
//	mMin = XMFLOAT3(0.0f, 0.0f, 0.0f);
//	mMax = XMFLOAT3(0.0f, 0.0f, 0.0f);
//	mCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
//}
//
//AABB::~AABB()
//{
//}
//
//void AABB::Generate(const XMFLOAT3* points, const unsigned int size)
//{
//	GetExtremes(points, size, mMin, mMax);
//
//	// min + max / 2 gives us the center.
//	mCenter = DirectX::XMFLOAT3
//	(
//		(mMin.x + mMax.x) / 2.0f,
//		(mMin.y + mMax.y) / 2.0f,
//		(mMin.z + mMax.z) / 2.0f
//	);
//}
//
//bool AABB::Intersect(const BoundingVolume* volume)
//{
//	const OBB* obb = dynamic_cast<const OBB*>(volume);
//	if (obb)
//	{
//		return IntersectOBB(volume);
//	}
//	const AABB* aabb = dynamic_cast<const AABB*>(volume);
//	if (aabb)
//	{
//		return IntersectAABB(volume);
//	}
//	const BoundingSphere* sphere = dynamic_cast<const BoundingSphere*>(volume);
//	if (sphere)
//	{
//		return IntersectSphere(volume);
//	}
//}
//
//void AABB::WorldTransform(const XMMATRIX& world)
//{
//}
//
//bool AABB::IntersectSphere(const BoundingVolume* volume)
//{
//	const BoundingSphere* sphere = dynamic_cast<const BoundingSphere*>(volume);
//	float dist_sq = Square(*sphere->GetRadius());
//
//	// Solution from stackoverflow, don't question it.
//	if (mCenter.x < mMin.x)
//	{
//		dist_sq -= Square(mCenter.x - mMin.x);
//	}
//	else if (mCenter.x > mMax.x)
//	{
//		dist_sq -= Square(mCenter.x - mMax.x);
//	}
//	if (mCenter.y < mMin.y)
//	{
//		dist_sq -= Square(mCenter.y - mMin.y);
//	}
//	else if (mCenter.y > mMax.y)
//	{
//		dist_sq -= Square(mCenter.y - mMax.y);
//	}
//	if (mCenter.z < mMin.z)
//	{
//		dist_sq -= Square(mCenter.z - mMin.z);
//	}
//	else if (mCenter.z > mMax.z)
//	{
//		dist_sq -= Square(mCenter.z - mMax.z);
//	}
//	return dist_sq > 0.0f;
//}
//
//bool AABB::IntersectOBB(const BoundingVolume* volume)
//{
//	return false;
//}
//
//bool AABB::IntersectAABB(const BoundingVolume* volume)
//{
//	const AABB* aabb = dynamic_cast<const AABB*>(volume);
//
//	const XMFLOAT3* other_min = aabb->GetMin();
//	const XMFLOAT3* other_max = aabb->GetMax();
//	// Standard evaluation that two AABBs overlap.
//	// Exits early if one check fails due to &&.
//	return (
//		mMax.x > other_min->x &&
//		mMin.x < other_max->x &&
//		mMax.y > other_min->y &&
//		mMin.y < other_max->y &&
//		mMax.z > other_min->z &&
//		mMin.z < other_max->z);
//}
//
//const XMFLOAT3* AABB::GetMin() const
//{
//	return &mMin;
//}
//
//const XMFLOAT3* AABB::GetMax() const
//{
//	return &mMax;
//}
//
//const XMFLOAT3* AABB::GetCenter() const
//{
//	return &mCenter;
//}