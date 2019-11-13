#pragma once

#include "ecsSystemIncludes.h"
#include "GraphicsECSComponents.h"

namespace ecs
{
	namespace systems
	{
		class RenderBufferResetSystem : public ECSSystem<RenderBufferResetSystem>
		{
		public:
			RenderBufferResetSystem();
			virtual ~RenderBufferResetSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		class UploadRenderBufferSystem : public ECSSystem<UploadRenderBufferSystem>
		{
		public:
			UploadRenderBufferSystem();
			virtual ~UploadRenderBufferSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		class PipelineShadowMapSystem : public ECSSystem<PipelineShadowMapSystem>
		{
		public:
			PipelineShadowMapSystem();
			virtual ~PipelineShadowMapSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		class PipelineForwardSystem : public ECSSystem<PipelineForwardSystem>
		{
		public:
			PipelineForwardSystem();
			virtual ~PipelineForwardSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		class ClearGPURenderSystem : public ECSSystem<ClearGPURenderSystem>
		{
		public:
			ClearGPURenderSystem();
			virtual ~ClearGPURenderSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};

		class ExecuteGPURenderSystem : public ECSSystem<ExecuteGPURenderSystem>
		{
		public:
			ExecuteGPURenderSystem();
			virtual ~ExecuteGPURenderSystem() {}

			void updateEntity(FilteredEntity& entity, float delta) override;
		};
	}
}