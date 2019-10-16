#include "CollisionFunctions.h"

void RevertMovement(XMFLOAT3& position, BoundingVolume* v1, BoundingVolume* v2)
{
	
}

void RevertMovement(XMFLOAT3& position, XMFLOAT3& velocity, const XMVECTOR& center_world, const XMVECTOR& colliding_center_world, const float& delta)
{
	// Getting a vector between the centers of the bounding volumes.
	DirectX::XMVECTOR v_diff = DirectX::XMVectorSubtract(center_world, colliding_center_world);
	DirectX::XMFLOAT3 diff;
	DirectX::XMStoreFloat3(&diff, v_diff);

	// Saving a 1 in the direction of the largest component in the vector.
	bool x = false;
	bool y = false;
	bool z = false;

	if (fabs(diff.x) > fabs(diff.y) && fabs(diff.x) > fabs(diff.z))
	{
		x = true;
	}
	else if (fabs(diff.y) > fabs(diff.x) && fabs(diff.y) > fabs(diff.z))
	{
		y = true;
	}
	else
	{
		z = true;
	}

	// Reverting the movement in that direction.
	position.x -= velocity.x * delta * x;
	position.y -= velocity.y * delta * y;
	position.z -= velocity.z * delta * z;

	// Resetting the velocity in that direction.
	velocity.x *= !x;
	velocity.y *= !y;
	velocity.z *= !z;
}

//void CreateOBB(DirectX::XMFLOAT3(&vertices)[8], const DirectX::XMFLOAT3& min_point, const DirectX::XMFLOAT3& max_point)
//{
//	// Creating the vertices for the box with the min and max points.
//
//	// Near z:	2 3
//	//			0 1
//	vertices[0] = min_point;
//	vertices[1] = DirectX::XMFLOAT3(max_point.x, min_point.y, min_point.z);
//	vertices[2] = DirectX::XMFLOAT3(min_point.x, max_point.y, min_point.z);
//	vertices[3] = DirectX::XMFLOAT3(max_point.x, max_point.y, min_point.z);
//
//	// Far z:	6 7
//	//			4 5
//	vertices[4] = DirectX::XMFLOAT3(min_point.x, min_point.y, max_point.z);
//	vertices[5] = DirectX::XMFLOAT3(max_point.x, min_point.y, max_point.z);
//	vertices[6] = DirectX::XMFLOAT3(min_point.x, max_point.y, max_point.z);
//	vertices[7] = max_point;
//}
//
//void GetExtremes(const DirectX::XMFLOAT3* points, const unsigned int n_points, DirectX::XMFLOAT3& min, DirectX::XMFLOAT3& max)
//{
//	// Creating min and max points to make a box.
//	min = DirectX::XMFLOAT3(INFINITY, INFINITY, INFINITY);
//	max = DirectX::XMFLOAT3(-INFINITY, -INFINITY, -INFINITY);
//
//	// Looping over each vertex to see if they contain
//	// the lowest or highest value in each axis.
//	for (int i = 0; i < n_points; i++)
//	{
//		DirectX::XMFLOAT3 current = points[i];
//		if (current.x < min.x)
//		{
//			min.x = current.x;
//		}
//		else if (current.x > max.x)
//		{
//			max.x = current.x;
//		}
//		if (current.y < min.y)
//		{
//			min.y = current.y;
//		}
//		else if (current.y > max.y)
//		{
//			max.y = current.y;
//		}
//		if (current.z < min.z)
//		{
//			min.z = current.z;
//		}
//		else if (current.z > max.z)
//		{
//			max.z = current.z;
//		}
//	}
//}
//
//void CreateBoundingSphere(const DirectX::XMFLOAT3* points, const unsigned int& n_points, const DirectX::XMFLOAT3& min_point, const DirectX::XMFLOAT3& max_point, float& radius, DirectX::XMFLOAT3& center)
//{
//	// min + max / 2 gives us the center.
//	center = DirectX::XMFLOAT3
//	(
//		(min_point.x + max_point.x) / 2.0f,
//		(min_point.y + max_point.y) / 2.0f,
//		(min_point.z + max_point.z) / 2.0f
//	);
//
//	// Find the biggest distance to a point in the group
//	// to get the radius of the smallest enclosing sphere.
//	float biggest_distance = 0.0f;
//	for (int i = 0; i < n_points; i++)
//	{
//		float distance = PhysicsHelpers::CalculateDistance(center, points[i]);
//		if (biggest_distance < distance)
//		{
//			biggest_distance = distance;
//		}
//	}
//	radius = biggest_distance;
//}
//
//bool AABBIntersect(const DirectX::XMFLOAT3& min1, const DirectX::XMFLOAT3& max1, const DirectX::XMFLOAT3& min2, const DirectX::XMFLOAT3& max2)
//{
//	// Standard evaluation that two AABBs overlap.
//	// Exits early if one check fails due to &&.
//	return (
//		max1.x > min2.x &&
//		min1.x < max2.x &&
//		max1.y > min2.y &&
//		min1.y < max2.y &&
//		max1.z > min2.z &&
//		min1.z < max2.z);
//}
//
//bool AABBIntersect(const DirectX::XMVECTOR& v_min1, const DirectX::XMVECTOR& v_max1, const DirectX::XMVECTOR& v_min2, const DirectX::XMVECTOR& v_max2)
//{
//	// Standard evaluation that two AABBs overlap.
//	// Exits early if one check fails due to &&.
//	DirectX::XMFLOAT3 min1;
//	DirectX::XMStoreFloat3(&min1, v_min1);
//	DirectX::XMFLOAT3 max1;
//	DirectX::XMStoreFloat3(&max1, v_max1);
//	DirectX::XMFLOAT3 min2;
//	DirectX::XMStoreFloat3(&min2, v_min2);
//	DirectX::XMFLOAT3 max2;
//	DirectX::XMStoreFloat3(&max2, v_max2);
//	return (
//		max1.x > min2.x &&
//		min1.x < max2.x &&
//		max1.y > min2.y &&
//		min1.y < max2.y &&
//		max1.z > min2.z &&
//		min1.z < max2.z);
//}
//
//bool SphereIntersect(const DirectX::XMFLOAT3& center1, const float& radius1, const DirectX::XMFLOAT3& center2, const float& radius2)
//{
//	// If the distance between the center points is greater than the sum of the radii, 
//	// the spheres intersect, with the difference being the overlap in the direction of
//	// the vector between the centers.
//	return CalculateDistance(center1, center2) < (radius1 + radius2);
//}
