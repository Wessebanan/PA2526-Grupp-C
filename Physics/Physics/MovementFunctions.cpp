#include "MovementFunctions.h"
void StaticMove(XMFLOAT3& pos, const XMFLOAT3& dir, const float& velocity, const float& delta)
{
	pos = DirectX::XMFLOAT3(
		pos.x + velocity * delta * dir.x,
		pos.y + velocity * delta * dir.y,
		pos.z + velocity * delta * dir.z);	
}

void SetDirection(XMFLOAT3& dir, const XMFLOAT3& forward, MovementInputs input)
{
	// Rotating its direction of movement based on current 
	// direction and input direction.
	DirectX::XMFLOAT3 direction = forward;
	RotateAroundY(direction, input);

	dir.x = direction.x;
	dir.z = direction.z;
}

void UpdateAcceleration(XMFLOAT3& acc, const XMFLOAT3& force, const float& weight, const XMFLOAT3& velocity, const float& dec)
{
	// a = F/m
	acc.x = force.x / weight;
	acc.z = force.z / weight;

	// Applying deceleration if velocity is greater than 0 and acceleration works opposite or not at all.
	if (fabs(velocity.x) > 0.001f && acc.x / velocity.x <= 0.0f)
	{
		// Reducing velocity by acceleration and time in the opposite direction of the velocity.
		acc.x -= Sign(velocity.x) * dec;
	}
	if (fabs(velocity.z) > 0.001f && acc.z / velocity.z <= 0.0f)
	{
		// Reducing velocity by acceleration and time in the opposite direction of the velocity.
		acc.z -= Sign(velocity.z) * dec;
	}
}

float HandleGravity(float& y_acc, float& y_vel, float& y_pos, const float& gravity, const float& max_vel, const float& delta)
{
	y_acc = -gravity;
	if (fabs(y_vel) < max_vel)
	{
		y_vel += y_acc * delta;
	}

	// Separate if rather than else for the specific moment max velocity is passed.
	if (fabs(y_vel) > max_vel)
	{
		y_vel = Sign(y_vel) * max_vel;
	}
	float change = y_vel * delta;
	y_pos += y_vel * delta;
	return change;
}
