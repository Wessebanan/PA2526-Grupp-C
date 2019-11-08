#include "CollisionObject.h"

CollisionObject::~CollisionObject()
{
	delete mNext; 
	mNext = nullptr;
}

CollisionInfo CollisionObject::GetCollisionInfo(OBB& obb)
{
	if (mType == COLLISION_AABB)
	{
		return GetAABBCollisionInfo(obb);
	}
	else if (mType == COLLISION_OBB)
	{
		return GetOBBCollisionInfo(obb);
	}
	else if (mType == COLLISION_SPHERE)
	{
		return GetSphereCollisionInfo(obb);
	}
	else if (mType == COLLISION_CYLINDER)
	{
		return GetCylinderCollisionInfo(obb);
	}
	return CollisionInfo();
}

CollisionInfo CollisionObject::GetOBBCollisionInfo(OBB& obb)
{
	OBB* object_obb = static_cast<OBB*>(mBoundingVolume);

	// Grabbing copies of obbs to check.
	OBB object_obb_copy = OBB(*object_obb);
	OBB obb_copy = OBB(obb);

	// Taking the inverse of the object obb's orientation to transform it into axis aligned.
	XMMATRIX object_obb_rotation = XMMatrixRotationQuaternion(XMLoadFloat4(&object_obb->Orientation));
	XMMATRIX object_obb_rotation_inverse = XMMatrixInverse(nullptr, object_obb_rotation);

	// Moving object obb to origin and placing bone obb in the same relative position.
	object_obb_copy.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMStoreFloat3(&obb_copy.Center, XMVectorSubtract(XMLoadFloat3(&object_obb->Center), XMLoadFloat3(&obb.Center)));

	// Transforming both with the inverse orientation, making object_obb_copy axis aligned.
	object_obb_copy.Transform(object_obb_rotation_inverse);
	obb_copy.Transform(object_obb_rotation_inverse);

	// Using copies to get collision info as if the object had an AABB.
	CollisionObject temp_object;
	AABB temp_aabb;
	temp_aabb.Center = object_obb_copy.Center;
	temp_aabb.Extents = object_obb_copy.Center;
	temp_object.mBoundingVolume = &temp_aabb;

	// Rotating normal back according to obb orientation for true orientation of normal.
	CollisionInfo return_info = temp_object.GetAABBCollisionInfo(obb_copy);	
	XMStoreFloat3(&return_info.mNormal, XMVector3TransformNormal(XMLoadFloat3(&return_info.mNormal), object_obb_rotation));
	return return_info;
}

CollisionInfo CollisionObject::GetAABBCollisionInfo(OBB& obb)
{
	AABB* aabb = static_cast<AABB*>(mBoundingVolume);
	size_t n_corners = 8;
	XMFLOAT3* corners = new XMFLOAT3[n_corners];
	obb.GetCorners(corners);

	// Grab direction of corner closest to center of AABB.
	float closest_distance = FLT_MAX;
	XMFLOAT3 closest_corner = XMFLOAT3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < n_corners; i++)
	{
		float distance = CalculateDistance(aabb->Center, corners[i]);
		if (distance < closest_distance)
		{
			closest_corner = corners[i];
			closest_distance = distance;
		}
	}
	delete[] corners;

	// Saving vector from center of aabb to closest corner of obb.
	XMFLOAT3 center_to_closest;
	XMStoreFloat3(&center_to_closest, XMVectorSubtract(XMLoadFloat3(&closest_corner), XMLoadFloat3(&aabb->Center)));

	// Saving the abs values of center_to_closest to find largest component.
	XMFLOAT3 abs_center_to_closest;
	XMStoreFloat3(&abs_center_to_closest, XMVectorAbs(XMLoadFloat3(&center_to_closest)));

	CollisionInfo return_info;

	// Determining the collision normal and overlap based on largest component of center_to_closest.
	if (abs_center_to_closest.x > abs_center_to_closest.y && abs_center_to_closest.x > abs_center_to_closest.z)
	{
		int sign = Sign(center_to_closest.x);
		return_info.mOverlap = aabb->Extents.x - abs_center_to_closest.x;
		return_info.mNormal.x = sign;
	}
	else if (abs_center_to_closest.z > abs_center_to_closest.x && abs_center_to_closest.z > abs_center_to_closest.y)
	{
		int sign = Sign(center_to_closest.z);
		return_info.mOverlap = aabb->Extents.z - abs_center_to_closest.z;
		return_info.mNormal.z = sign;
	}
	else
	{
		int sign = Sign(center_to_closest.y);
		return_info.mOverlap = aabb->Extents.y - abs_center_to_closest.y;
		return_info.mNormal.y = sign;
	}

	return return_info;
}

CollisionInfo CollisionObject::GetSphereCollisionInfo(OBB& obb)
{
	Sphere* sphere = static_cast<Sphere*>(mBoundingVolume);
	size_t n_corners = 8;
	XMFLOAT3* corners = new XMFLOAT3[n_corners];
	obb.GetCorners(corners);

	// Grab direction of corner closest to center of Sphere.
	float closest_distance = FLT_MAX;
	XMFLOAT3 closest_corner = XMFLOAT3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < n_corners; i++)
	{
		float distance = CalculateDistance(sphere->Center, corners[i]);
		if (distance < closest_distance)
		{
			closest_corner = corners[i];
			closest_distance = distance;
		}
	}
	delete[] corners;

	// Saving vector from center of aabb to closest corner of obb.
	XMVECTOR vec_center_to_closest = XMVectorSubtract(XMLoadFloat3(&closest_corner), XMLoadFloat3(&sphere->Center));

	CollisionInfo return_info;
	
	// Taking normal as the normalized vector from the closest corner to sphere center.
	return_info.mNormal;
	XMStoreFloat3(&return_info.mNormal, XMVector3Normalize(vec_center_to_closest));

	// Taking the overlap as the difference between the sphere radius and length of the vector.
	return_info.mOverlap = sphere->Radius - XMVectorGetX(XMVector3Length(vec_center_to_closest));

	return return_info;
}

CollisionInfo CollisionObject::GetCylinderCollisionInfo(OBB& obb)
{
	return CollisionInfo();
}
