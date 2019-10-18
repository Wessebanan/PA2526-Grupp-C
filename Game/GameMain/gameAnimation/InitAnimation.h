#pragma once
#include "Mesh.h"
struct SkinningShaderProgramInput
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 boneMatrices[63];
};

void UpdateAnimation(ModelLoader::Skeleton* skeleton, SkinningShaderProgramInput* skin_shader_program_input, UINT frame)
{
	for (unsigned int i = 0; i < 12; ++i)
	{
		// Avoid frame overflow
		frame = frame % skeleton->frameCount;

		memcpy(skin_shader_program_input[i].boneMatrices, &skeleton->animationData[frame * skeleton->jointCount], skeleton->jointCount * sizeof(XMFLOAT4X4));

	}
}


void InitAnimation(ModelLoader::Skeleton* skeleton, SkinningShaderProgramInput* skin_shader_program_input)
{
	// Load animation data for first frame
	for (unsigned int i = 0; i < 12; ++i)
	{

		memcpy(
			&skin_shader_program_input[i].boneMatrices,
			skeleton->animationData,
			skeleton->jointCount * sizeof(DirectX::XMFLOAT4X4));

	}

	UpdateAnimation(skeleton, skin_shader_program_input, 0);
}
