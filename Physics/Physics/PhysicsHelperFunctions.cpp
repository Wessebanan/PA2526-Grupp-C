#include "PhysicsHelperFunctions.h"


float PhysicsHelpers::CalculateDistance(const DirectX::XMFLOAT3 &p1, const DirectX::XMFLOAT3 &p2)
{
	float x_diff = p1.x - p2.x;
	float y_diff = p1.y - p2.y;
	float z_diff = p1.z - p2.z;

	// Returns the floating point distance according to the distance formula.
	return sqrt(pow(x_diff, 2) + pow(y_diff, 2) + pow(z_diff, 2));
}

float PhysicsHelpers::CalculateDistance(const DirectX::XMVECTOR& p1, const DirectX::XMVECTOR& p2)
{
	DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(p1, p2);
	DirectX::XMVECTOR diff_length = DirectX::XMVector3Length(diff);
	float distance = 0.0f;
	DirectX::XMStoreFloat(&distance, diff_length);
	return distance;
}

float PhysicsHelpers::CalculateVectorLength(const DirectX::XMFLOAT3 vec)
{
	return sqrt(pow(vec.x,2)+pow(vec.y,2)+pow(vec.z,2));
}

void PhysicsHelpers::RotateAroundY(DirectX::XMFLOAT3& vec, int degrees)
{
	float cos_rotation = (float)cos(degrees * PI / 180.0);
	float sin_rotation = (float)sin(degrees * PI / 180.0);

	// Takes a copy for the rotation calculation.
	DirectX::XMFLOAT3 vec_copy = vec;

	// Rotation application around y-axis using the forward vector of the movement component.
	vec.x = vec_copy.x * cos_rotation - vec_copy.z * sin_rotation;
	vec.z = vec_copy.x * sin_rotation + vec_copy.z * cos_rotation;
	
	// If either is very close to 0, just make it 0.
	const float ABS_ERROR = pow(10, -10);
	if (fabs(vec.x) < ABS_ERROR)
	{
		vec.x = 0.0f;
	}
	if (fabs(vec.z) < ABS_ERROR)
	{
		vec.z = 0.0f;
	}
}

int PhysicsHelpers::Sign(float f)
{
	return (f > 0.0f) - (f < 0.0f);
}

