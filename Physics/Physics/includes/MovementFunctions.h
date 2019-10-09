#pragma once
#include <DirectXMath.h>
#include "PhysicsHelperFunctions.h"

using namespace DirectX;
using namespace PhysicsHelpers;
// MovementInputs are used to move entities in various directions.
enum MovementInputs
{
	FORWARD = 0,
	RIGHT = 90,
	LEFT = -90,
	BACKWARD = 180
};

// Moves statically from pos along dir with velocity * delta
void StaticMove(XMFLOAT3& pos, const XMFLOAT3& dir, const float& velocity, const float& delta);

void SetDirection(XMFLOAT3& dir, const XMFLOAT3 &forward, MovementInputs input);

void UpdateAcceleration(XMFLOAT3 &acc, const XMFLOAT3 &force, const float &weight, const XMFLOAT3 &velocity, const float &dec);

void HandleGravity(float&y_acc, float&y_vel, float&y_pos, const float &gravity, const float &max_vel, const float &delta);