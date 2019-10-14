#pragma once
#include <DirectXMath.h>

struct AnimatedModelBuffer
{
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 boneMatrices[63];
};