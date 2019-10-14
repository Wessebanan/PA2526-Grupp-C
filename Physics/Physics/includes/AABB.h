//#pragma once
//#include "BoundingVolume.h"
//#include "OBB.h"
//#include "BoundingSphere.h"
//
//class AABB : public BoundingVolume
//{
//public:
//	AABB();
//	~AABB();
//	void Generate(const XMFLOAT3* points, const unsigned int size);
//
//	bool Intersect(const BoundingVolume* volume);
//	void WorldTransform(const XMMATRIX& world);
//
//	const XMFLOAT3* GetMin() const;
//	const XMFLOAT3* GetMax() const;
//	const XMFLOAT3* GetCenter() const;
//
//private:
//	bool IntersectSphere(const BoundingVolume* volume);
//	bool IntersectOBB(const BoundingVolume* volume);
//	bool IntersectAABB(const BoundingVolume* volume);
//
//	XMFLOAT3 mMin;
//	XMFLOAT3 mMax;
//	XMFLOAT3 mCenter;
//};