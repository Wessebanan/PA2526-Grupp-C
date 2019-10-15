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

	inline float Square(float f);
} // helper_functions
