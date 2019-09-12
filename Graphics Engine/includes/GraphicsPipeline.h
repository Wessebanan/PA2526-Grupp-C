#pragma once

#include "D3D11Header.h"

namespace graphics
{
	class GraphicsPipeline
	{
		friend class RenderContext;
		friend class DeviceInterface;

	public:
		GraphicsPipeline();
		~GraphicsPipeline();

	private:
		void Initialize(
			ID3D11Device4* pDevice4,
			const std::string& vertexShader,
			const std::string& pixelShader);

		void Release();

		ID3D11InputLayout* m_pLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;
	};
}