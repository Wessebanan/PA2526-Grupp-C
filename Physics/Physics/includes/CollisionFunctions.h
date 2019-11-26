#pragma once
#include <DirectXMath.h>
#include "PhysicsHelperFunctions.h"
#include <DirectXCollision.h>
#include "BoundingVolume.h"

using namespace DirectX;
using namespace PhysicsHelpers;

void RevertMovement(XMFLOAT3 &position, XMFLOAT3 &velocity, const AABB *colliding_world, const AABB *collided_world, const float &delta);