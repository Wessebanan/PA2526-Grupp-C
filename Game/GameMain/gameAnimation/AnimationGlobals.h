#pragma once

#include "rendering/RenderManager.h"
#include <Mesh.h>
struct SkinPerObjectData
{
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 boneMatrices[63];
};