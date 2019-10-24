#pragma once

#include "ecsComponentIncludes.h"
#include "../../Graphics/includes/RenderManager.h"
#include "../../Graphics/includes/MeshManager.h"
#include "RenderBuffer.h"
#include "ShadowMapPipeline.h"
#include "ForwardRenderingPipeline.h"

namespace ecs
{
	namespace components
	{
		struct RenderManagerComponent : public ECSComponent<RenderManagerComponent>
		{
			graphics::RenderManager mgr;
		};

		struct MeshManagerComponent : public ECSComponent<MeshManagerComponent>
		{
			graphics::MeshManager mgr;
		};

		struct RenderBufferComponent : public ECSComponent<RenderBufferComponent>
		{
			graphics::RenderBuffer buffer;
			UINT bufferSize;
		};

		struct PipelineShadowMapComponent : public ECSComponent<PipelineShadowMapComponent>
		{
			graphics::SHADOW_MAP_PIPELINE_DESC pipelineDesc;
			graphics::SHADOW_MAP_PIPELINE_DATA data;
			UINT pipeline;
		};

		struct PipelineForwardComponent : public ECSComponent<PipelineForwardComponent>
		{
			graphics::FORWARD_RENDERING_PIPELINE_DESC pipelineDesc;
			graphics::FORWARD_RENDERING_PIPELINE_DATA data;
			UINT pipeline;
		};
	}
}