#pragma once
#include "BoundingVolume.h"

#pragma region Intersection

/*
	One test for each combination of bounding volumes.

	For each test that does not involve a cylinder, it's
	basically checking that the inheritance structure does
	not crash and burn since they DirectX intersection
	tests.
*/
TEST(Intersection, SphereSphere)
{
	BoundingVolume* p_bv1 = nullptr;
	BoundingVolume* p_bv2 = nullptr;

	p_bv1 = new Sphere;
	p_bv2 = new Sphere;

	Sphere* p_sphere1 = static_cast<Sphere*>(p_bv1);
	Sphere* p_sphere2 = static_cast<Sphere*>(p_bv2);

	p_sphere1->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	p_sphere2->Center = XMFLOAT3(1.0f, 1.0f, 1.0f);

	p_sphere1->Radius = 0.5f;
	p_sphere2->Radius = 0.5f;

	// Using base class for intersection test.

	// Expecting false since they don't intersect.
	EXPECT_FALSE(p_bv1->Intersects(p_bv2));
	
	// Moving sphere 2 to exactly touch the edge of sphere 1.
	p_sphere2->Center = XMFLOAT3(1.0f / sqrt(3.0f), 1.0f / sqrt(3.0f), 1.0f / sqrt(3.0f));

	// Trying bv2->bv1 just for fun. Now they intersect.
	EXPECT_TRUE(p_bv2->Intersects(p_bv1));

	delete p_bv1;
	delete p_bv2;
}
TEST(Intersection, SphereAABB)
{
	BoundingVolume* p_bv1 = nullptr;
	BoundingVolume* p_bv2 = nullptr;

	p_bv1 = new Sphere;
	p_bv2 = new AABB;

	Sphere* p_sphere = static_cast<Sphere*>(p_bv1);
	AABB* p_aabb = static_cast<AABB*>(p_bv2);

	p_sphere->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	p_aabb->Center = XMFLOAT3(1.0f, 1.0f, 1.0f);

	p_sphere->Radius = 0.5f;
	p_aabb->Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);

	// Using base class for intersection test.

	// Expecting false since they don't intersect.
	EXPECT_FALSE(p_bv1->Intersects(p_bv2));

	// Moving aabb so one corner barely penetrates the sphere.
	p_aabb->Center = XMFLOAT3(1.0f / sqrt(3.0f), 1.0f / sqrt(3.0f), 1.0f / sqrt(3.0f));

	// Trying bv2->bv1 just for fun. Now they intersect.
	EXPECT_TRUE(p_bv2->Intersects(p_bv1));

	delete p_bv1;
	delete p_bv2;
}
TEST(Intersection, SphereOBB)
{
	BoundingVolume* p_bv1 = nullptr;
	BoundingVolume* p_bv2 = nullptr;

	p_bv1 = new Sphere;
	p_bv2 = new OBB;

	Sphere* p_sphere = static_cast<Sphere*>(p_bv1);
	OBB* p_obb = static_cast<OBB*>(p_bv2);

	p_sphere->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	p_obb->Center = XMFLOAT3(1.0f, 1.0f, 1.0f);

	p_sphere->Radius = 0.5f;
	p_obb->Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);

	// Using base class for intersection test.

	// Expecting false since they don't intersect.
	EXPECT_FALSE(p_bv1->Intersects(p_bv2));

	// Moving obb so one corner barely penetrates the sphere.
	p_obb->Center = XMFLOAT3(1.0f / sqrt(3.0f), 1.0f / sqrt(3.0f), 1.0f / sqrt(3.0f));

	// Trying bv2->bv1 just for fun. Now they intersect.
	EXPECT_TRUE(p_bv2->Intersects(p_bv1));

	delete p_bv1;
	delete p_bv2;
}
TEST(Intersection, SphereCylinder)
{
	BoundingVolume* p_bv1 = nullptr;
	BoundingVolume* p_bv2 = nullptr;

	p_bv1 = new Sphere;
	p_bv2 = new Cylinder;

	Sphere* p_sphere = static_cast<Sphere*>(p_bv1);
	Cylinder* p_cylinder = static_cast<Cylinder*>(p_bv2);

	p_sphere->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	p_cylinder->mCenter = XMFLOAT3(0.0f, 1.6f, 0.0f);

	p_sphere->Radius = 0.5f;
	p_cylinder->mRadius = 0.5f;

	p_cylinder->mExtentsY = 1.0f;

	// Testing y intersection.
	p_bv1->Intersects(p_bv2);
	EXPECT_FALSE(p_bv1->Intersects(p_bv2));

	// Moving cylinder so the sphere intersects bottom of cylinder.
	p_cylinder->mCenter = XMFLOAT3(0.0f, 1.0f, 0.0f);

	EXPECT_TRUE(p_bv2->Intersects(p_bv1));

	// Moving cylinder so the sphere intersects top of cylinder.
	p_cylinder->mCenter = XMFLOAT3(0.0f, -1.0f, 0.0f);
	
	EXPECT_TRUE(p_bv1->Intersects(p_bv2));

	// Moving sphere so it intersects side of cylinder.
	p_sphere->Center = XMFLOAT3(0.99f, -1.0f, 0.0f);

	EXPECT_TRUE(p_bv1->Intersects(p_bv2));

	delete p_bv1;
	delete p_bv2;
}
TEST(Intersection, AABBAABB)
{
	BoundingVolume* p_bv1 = nullptr;
	BoundingVolume* p_bv2 = nullptr;

	p_bv1 = new AABB;
	p_bv2 = new AABB;

	AABB* p_aabb1 = static_cast<AABB*>(p_bv1);
	AABB* p_aabb2 = static_cast<AABB*>(p_bv2);

	p_aabb1->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	p_aabb2->Center = XMFLOAT3(1.01f, 1.01f, 1.01f);

	p_aabb1->Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);
	p_aabb2->Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);

	EXPECT_FALSE(p_bv1->Intersects(p_bv2));
	
	p_aabb2->Center = XMFLOAT3(1.0f, 1.0f, 1.0f);

	EXPECT_TRUE(p_bv2->Intersects(p_bv1));

	delete p_bv1;
	delete p_bv2;
}
TEST(Intersection, AABBOBB)
{
	BoundingVolume* p_bv1 = nullptr;
	BoundingVolume* p_bv2 = nullptr;

	p_bv1 = new AABB;
	p_bv2 = new OBB;

	AABB* p_aabb = static_cast<AABB*>(p_bv1);
	OBB* p_obb = static_cast<OBB*>(p_bv2);

	p_aabb->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	p_obb->Center = XMFLOAT3(1.01f, 1.01f, 1.01f);

	p_aabb->Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);
	p_obb->Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);

	EXPECT_FALSE(p_bv1->Intersects(p_bv2));

	p_obb->Center = XMFLOAT3(1.0f, 1.0f, 1.0f);

	EXPECT_TRUE(p_bv2->Intersects(p_bv1));

	delete p_bv1;
	delete p_bv2;
}
TEST(Intersection, AABBCylinder)
{
	BoundingVolume* p_bv1 = nullptr;
	BoundingVolume* p_bv2 = nullptr;

	p_bv1 = new AABB;
	p_bv2 = new Cylinder;

	AABB* p_aabb = static_cast<AABB*>(p_bv1);
	Cylinder* p_cylinder = static_cast<Cylinder*>(p_bv2);

	p_aabb->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	p_aabb->Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);

	p_cylinder->mCenter = XMFLOAT3(0.0f, 1.01f, 0.0f);
	p_cylinder->mExtentsY = 0.5f;
	p_cylinder->mRadius = 0.5f;

	EXPECT_FALSE(p_bv1->Intersects(p_bv2));

	// Moving cylinder so the aabb intersects bottom of cylinder.
	p_cylinder->mCenter = XMFLOAT3(0.0f, 0.99f, 0.0f);

	EXPECT_TRUE(p_bv2->Intersects(p_bv1));

	// Moving cylinder so the aabb intersects top of cylinder.
	p_cylinder->mCenter = XMFLOAT3(0.0f, -0.99f, 0.0f);

	EXPECT_TRUE(p_bv1->Intersects(p_bv2));

	// Moving aabb so it intersects side of cylinder.
	p_aabb->Center = XMFLOAT3(0.99f, -1.0f, 0.0f);

	EXPECT_TRUE(p_bv1->Intersects(p_bv2));

	delete p_bv1;
	delete p_bv2;
}
TEST(Intersection, OBBOBB)
{
	BoundingVolume* p_bv1 = nullptr;
	BoundingVolume* p_bv2 = nullptr;

	p_bv1 = new OBB;
	p_bv2 = new OBB;

	OBB* p_obb1 = static_cast<OBB*>(p_bv1);
	OBB* p_obb2 = static_cast<OBB*>(p_bv2);

	p_obb1->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	p_obb2->Center = XMFLOAT3(1.01f, 1.01f, 1.01f);

	p_obb1->Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);
	p_obb2->Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);

	EXPECT_FALSE(p_bv1->Intersects(p_bv2));

	p_obb2->Center = XMFLOAT3(1.0f, 1.0f, 1.0f);

	EXPECT_TRUE(p_bv2->Intersects(p_bv1));

	delete p_bv1;
	delete p_bv2;
}
TEST(Intersection, OBBCylinder)
{
	BoundingVolume* p_bv1 = nullptr;
	BoundingVolume* p_bv2 = nullptr;

	p_bv1 = new OBB;
	p_bv2 = new Cylinder;

	OBB* p_obb = static_cast<OBB*>(p_bv1);
	Cylinder* p_cylinder = static_cast<Cylinder*>(p_bv2);

	p_obb->Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	p_obb->Extents = XMFLOAT3(0.5f, 0.5f, 0.5f);

	p_cylinder->mCenter = XMFLOAT3(0.0f, 1.01f, 0.0f);
	p_cylinder->mExtentsY = 0.5f;
	p_cylinder->mRadius = 1.0f;

	EXPECT_FALSE(p_bv1->Intersects(p_bv2));

	// Intersection from the bottom.
	p_cylinder->mCenter = XMFLOAT3(0.0f, 0.99f, 0.0f);

	EXPECT_TRUE(p_bv2->Intersects(p_bv1));

	// Intersection from the top.
	p_cylinder->mCenter = XMFLOAT3(0.0f, -0.99f, 0.0f);

	EXPECT_TRUE(p_bv1->Intersects(p_bv2));

	// Intersection from the side.

	// Rotate obb 45 degrees around y to get a corner intersecting.
	XMVECTOR axis = XMVectorZero();
	axis = XMVectorSetY(axis, 1.0f);
	XMMATRIX rot = XMMatrixRotationAxis(axis, 45.0f * PI / 180.0f);

	p_obb->Transform(rot);
	
	// Move obb so it barely intersects cylinder circle.
	p_obb->Center = p_cylinder->mCenter;
	p_obb->Center.x = 1.0f;

	// Adjusting height so at least one corner intersects.
	p_obb->Center.y -= 0.1f;

	EXPECT_TRUE(p_bv2->Intersects(p_bv1));

	delete p_bv1;
	delete p_bv2;
}
TEST(Intersection, CylinderCylinder)
{
	BoundingVolume* p_bv1 = nullptr;
	BoundingVolume* p_bv2 = nullptr;

	p_bv1 = new Cylinder;
	p_bv2 = new Cylinder;

	Cylinder* p_cylinder1 = static_cast<Cylinder*>(p_bv1);
	Cylinder* p_cylinder2 = static_cast<Cylinder*>(p_bv2);

	p_cylinder1->mCenter = XMFLOAT3(0.0f, 0.0f, 0.0f);
	p_cylinder2->mCenter = XMFLOAT3(0.0f, 1.01f, 0.0f);

	p_cylinder1->mExtentsY = 0.5f;
	p_cylinder2->mExtentsY = 0.5f;

	p_cylinder1->mRadius = 0.5f;
	p_cylinder2->mRadius = 0.5f;

	// No intersection at first (top of cyl1 is equal to bottom of cyl2)
	EXPECT_FALSE(p_bv1->Intersects(p_bv2));

	// Intersection (top of cyl1 with bottom of cyl2)
	p_cylinder2->mCenter = XMFLOAT3(0.0f, 0.99f, 0.0f);

	EXPECT_TRUE(p_bv2->Intersects(p_bv1));

	// Side to side intersection
	p_cylinder2->mCenter = p_cylinder1->mCenter;
	p_cylinder2->mCenter.x = 1.0f;

	EXPECT_TRUE(p_bv2->Intersects(p_bv1));

	delete p_bv1;
	delete p_bv2;
}