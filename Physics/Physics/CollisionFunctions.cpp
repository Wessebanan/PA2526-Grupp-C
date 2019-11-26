#include "CollisionFunctions.h"

void RevertMovement(XMFLOAT3& position, XMFLOAT3& velocity, const AABB* colliding_world, const AABB* collided_world, const float& delta)
{
	// Getting a vector between the centers of the bounding volumes.
	DirectX::XMVECTOR v_diff = DirectX::XMVectorSubtract(XMLoadFloat3(&colliding_world->Center), XMLoadFloat3(&collided_world->Center));
	DirectX::XMFLOAT3 diff;
	DirectX::XMStoreFloat3(&diff, v_diff);

	// Getting the magnitude of the differences in each direction.
	XMFLOAT3 fabs_diff = XMFLOAT3(fabs(diff.x), fabs(diff.y), fabs(diff.z));

	// Checking that the object is not trying to move away from the colliding object.
	XMVECTOR v_velocity = XMLoadFloat3(&velocity);

	v_velocity = XMVector3Normalize(v_velocity);
	v_diff = XMVector3Normalize(v_diff);

	// XMVector3Dot replicates dot product into each component.
	XMVECTOR dot = XMVector3Dot(v_velocity, v_diff);

	// If the dot product is positive, do not revert movement.
	if (XMVectorGetX(dot) >= 0.0f)
	{
		return;
	}

	bool x = false;
	bool y = false;
	bool z = false;

	float overlap = 0.0f;
	int sign = 0;
	// Saving a 1 in the direction of the largest component in the vector and getting the overlap in that direction.
	if (/*fabs_diff.x > fabs_diff.y &&*/ fabs_diff.x > fabs_diff.z)
	{
		sign = Sign(diff.x);
		overlap = colliding_world->Extents.x + collided_world->Extents.x - fabs_diff.x;
		x = true;
	}
	else if (fabs_diff.z > fabs_diff.x /*&& fabs_diff.z > fabs_diff.y*/)
	{
		sign = Sign(diff.z);
		overlap = colliding_world->Extents.z + collided_world->Extents.z - fabs_diff.z;
		z = true;
	}
	else
	{
		sign = Sign(diff.y);
		overlap = colliding_world->Extents.y + collided_world->Extents.y - fabs_diff.y;
		y = true;
	}

	float movement = sign * overlap;

	// Reverting the movement in that direction.
	position.x += movement * (float)x;
	position.y += movement * (float)y;
	position.z += movement * (float)z;

	// Flipping the velocity in that direction.
	velocity.x *= (float)(!x*2.0f)-1.0f;
	velocity.y *= (float)(!y*2.0f)-1.0f;
	velocity.z *= (float)(!z*2.0f)-1.0f;
}
