#pragma once
#include <DirectXMath.h>
#define PI 3.14159265358979323846
namespace PhysicsHelpers
{
	// Returns the distance between two points as a float.
	float CalculateDistance(const DirectX::XMFLOAT3 &p1, const DirectX::XMFLOAT3 &p2);
	float CalculateDistance(const DirectX::XMVECTOR& p1, const DirectX::XMVECTOR& p2);

	// Returns the vector length as a float.
	float CalculateVectorLength(const DirectX::XMFLOAT3 vec);

	void RotateAroundY(DirectX::XMFLOAT3 &vec, int degrees);

	// Returns 1 for positive, -1 for negative and 0 for 0
	int Sign(float f);

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
	
	DirectX::XMFLOAT3 operator+(const DirectX::XMFLOAT3& p1, const DirectX::XMFLOAT3& p2);

	// Takes the extreme points of two AABBs and evaluates if they intersect.
	bool AABBIntersect(const DirectX::XMFLOAT3 &min1, const DirectX::XMFLOAT3 &max1, const DirectX::XMFLOAT3 &min2, const DirectX::XMFLOAT3 &max2);
	bool AABBIntersect(const DirectX::XMVECTOR& v_min1, const DirectX::XMVECTOR& v_max1, const DirectX::XMVECTOR& v_min2, const DirectX::XMVECTOR& v_max2);

	// Takes center and radius of two spheres and evaluates if they intersect.
	bool SphereIntersect(const DirectX::XMFLOAT3& center1, const float& radius1, const DirectX::XMFLOAT3& center2, const float& radius2);
} // helper_functions
