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

	// Using base class for intersection test.

	// Expecting false since they don't intersect.
	p_bv1->Intersects(p_bv2);
	EXPECT_FALSE(p_bv1->Intersects(p_bv2));

	// Moving cylinder so one corner barely penetrates the sphere.
	p_cylinder->mCenter = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// Trying bv2->bv1 just for fun. Now they intersect.
	EXPECT_TRUE(p_bv2->Intersects(p_bv1));

	p_sphere->Center = XMFLOAT3(1.0f, 1.0f, 0.0f);


	delete p_bv1;
	delete p_bv2;
}
TEST(Intersection, AABBAABB)
{

}
TEST(Intersection, AABBOBB)
{

}
TEST(Intersection, AABBCylinder)
{

}
TEST(Intersection, OBBOBB)
{

}
TEST(Intersection, OBBCylinder)
{

}
TEST(Intersection, CylinderCylinder)
{

}