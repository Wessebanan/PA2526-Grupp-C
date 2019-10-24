#pragma once

#pragma once

#pragma once

#include "Pipeline.h"

#include <vector>
#include <DirectXMath.h>

namespace graphics
{
	class StateManager
	{
	public:
		StateManager();
		~StateManager();

		/*
			Initialize with a window that will be the back buffer
		*/
		HRESULT Initialize();

		/*
			Create Pipeline (Every pipeline has their own structs for description)
			: Returns UINT_MAX if failed
		*/
		UINT CreatePipelineState(StatePipeline* pPipeline, const void* pDescription);

		/*
			Update pipeline data (Every pipeline has their own structs for data)
		*/
		void UpdatePipelineState(const UINT pipeline, const void* pStateData);

		/*
			Execute desired pipeline (will use all created programs)
		*/
		void SetPipelineState(const UINT pipeline);

		void Destroy();
	private:
		std::vector<StatePipeline*> m_settings;

		ID3D11Device4* m_pDevice4;
		ID3D11DeviceContext4* m_pContext4;
	};
}