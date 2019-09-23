#pragma once

#include "D3D11Header.h"

namespace graphics
{
	class GraphicsPipeline
	{
		friend class RenderContext;
		friend class DeviceInterface;
		friend class GraphicsPipelineArray;

	public:
		GraphicsPipeline();
		~GraphicsPipeline();

	private:

		static UINT NUM_ALLOCATED;

		int Initialize(
			ID3D11Device4* pDevice4,
			const std::string& vertexShader,
			const std::string& pixelShader);

		void Release();

		ID3D11InputLayout* m_pLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;

		UINT m_id;
	};		

	class GraphicsPipelineArray
	{
	public:
		GraphicsPipelineArray();
		~GraphicsPipelineArray();

		int Initialize(
			ID3D11Device4* pDevice4, 
			const UINT capacity);

		void Release();

		int CreateGraphicsPipeline(
			ID3D11Device4* pDevice4,
			const std::string& vertexShader,
			const std::string& pixelShader,
			GraphicsPipeline** ppPipeline);

		bool DeleteGraphicsPipeline(GraphicsPipeline* pPipeline);

	private:
		char* m_pData;
		GraphicsPipeline* m_pPipelines;
		UINT* m_pIsAvailableArray;

		UINT
			m_capacity,
			m_count,
			m_start;
	};
}