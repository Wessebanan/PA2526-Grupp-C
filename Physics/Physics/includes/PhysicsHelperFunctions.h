#pragma once
#include <DirectXMath.h>

namespace physics_helper_functions
{
	// Returns the distance between two points as a float.
	float CalculateDistance(DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 p2);

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
	void CreateOBB(DirectX::XMFLOAT3 (&vertices)[8], DirectX::XMFLOAT3 min_point, DirectX::XMFLOAT3 max_point);

} // helper_functions
