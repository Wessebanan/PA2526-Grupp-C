#include "PhysicsHelperFunctions.h"


float PhysicsHelpers::CalculateDistance(const DirectX::XMFLOAT3 &p1, const DirectX::XMFLOAT3 &p2)
{
	float x_diff = p1.x - p2.x;
	float y_diff = p1.y - p2.y;
	float z_diff = p1.z - p2.z;

	// Returns the floating point distance according to the distance formula.
	return sqrt(pow(x_diff, 2) + pow(y_diff, 2) + pow(z_diff, 2));
}

void PhysicsHelpers::CreateOBB(DirectX::XMFLOAT3(&vertices)[8], const DirectX::XMFLOAT3 &min_point, const DirectX::XMFLOAT3 &max_point)
{
	// Creating the vertices for the box with the min and max points.

	// Near z:	2 3
	//			0 1
	vertices[0] = min_point;
	vertices[1] = DirectX::XMFLOAT3(max_point.x, min_point.y, min_point.z);
	vertices[2] = DirectX::XMFLOAT3(min_point.x, max_point.y, min_point.z);
	vertices[3] = DirectX::XMFLOAT3(max_point.x, max_point.y, min_point.z);
	
	// Far z:	6 7
	//			4 5
	vertices[4] = DirectX::XMFLOAT3(min_point.x, min_point.y, max_point.z);
	vertices[5] = DirectX::XMFLOAT3(max_point.x, min_point.y, max_point.z);
	vertices[6] = DirectX::XMFLOAT3(min_point.x, max_point.y, max_point.z);
	vertices[7] = max_point;
}

void PhysicsHelpers::GetExtremes(const DirectX::XMFLOAT3* points, const unsigned int n_points, DirectX::XMFLOAT3& min, DirectX::XMFLOAT3& max)
{
	// Creating min and max points to make a box.
	min = DirectX::XMFLOAT3(INFINITY, INFINITY, INFINITY);
	max = DirectX::XMFLOAT3(-INFINITY, -INFINITY, -INFINITY);

	// Looping over each vertex to see if they contain
	// the lowest or highest value in each axis.
	for (int i = 0; i < n_points; i++)
	{
		DirectX::XMFLOAT3 current = points[i];
		if (current.x < min.x)
		{
			min.x = current.x;
		}
		else if (current.x > max.x)
		{
			max.x = current.x;
		}
		if (current.y < min.y)
		{
			min.y = current.y;
		}
		else if (current.y > max.y)
		{
			max.y = current.y;
		}
		if (current.z < min.z)
		{
			min.z = current.z;
		}
		else if (current.z > max.z)
		{
			max.z = current.z;
		}
	}
}

void CreateBoundingSphere(const DirectX::XMFLOAT3* points, const unsigned int& n_points, const DirectX::XMFLOAT3& min_point, const DirectX::XMFLOAT3& max_point, float& radius, DirectX::XMFLOAT3& center)
{
	// min + max / 2 gives us the center.
	center = DirectX::XMFLOAT3
	(
		(min_point.x + max_point.x) / 2.0f,
		(min_point.y + max_point.y) / 2.0f,
		(min_point.z + max_point.z) / 2.0f
	);
	
	// Find the biggest distance to a point in the group
	// to get the radius of the smallest enclosing sphere.
	float biggest_distance = 0.0f;
	for (int i = 0; i < n_points; i++)
	{
		float distance = PhysicsHelpers::CalculateDistance(center, points[i]);
		if (biggest_distance < distance)
		{
			biggest_distance = distance;
		}
	}
	radius = biggest_distance;
}
