#pragma once
#include <DirectXMath.h>

namespace PhysicsHelpers
{
	// Returns the distance between two points as a float.
	float CalculateDistance(const DirectX::XMFLOAT3 &p1, const DirectX::XMFLOAT3 &p2);

	// Fills the vertices parameter with OBB vertices according to:
	/*
	   6-------7
	  /|      /|
	 / |     / |
	2--|----3  |
	|  4----|--5
	| /     | /
	0-------1		|: y, -: x /: z
	*/
	void CreateOBB(DirectX::XMFLOAT3 (&vertices)[8], const DirectX::XMFLOAT3 &min_point, const DirectX::XMFLOAT3 &max_point);

	// Takes a group of points (vertices) and stores the extreme values in each axis in two points.
	void GetExtremes(const DirectX::XMFLOAT3* points, const unsigned int n_points, DirectX::XMFLOAT3& min, DirectX::XMFLOAT3& max);

	// Takes the extreme points of a vertex group and writes the resulting center and radius to parameters center and radius.
	void CreateBoundingSphere(const DirectX::XMFLOAT3* points, const unsigned int &n_points, const DirectX::XMFLOAT3& min_point, const DirectX::XMFLOAT3& max_point, float& radius, DirectX::XMFLOAT3& center);
} // helper_functions
