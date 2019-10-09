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

		int Initialize(
			ID3D11Device4* pDevice4,
			const std::string& vertexShader,
			const std::string& pixelShader);

		void Release();

		int CreateInputLayout(
			ID3D11Device4* pDevice4,
			ID3DBlob* pVertexShader);

		bool CreateVertexShader(
			ID3D11Device4* pDevice4,
			const std::string& shaderCode);

		bool CreatePixelShader(
			ID3D11Device4* pDevice4,
			const std::string& shaderCode);

		ID3D11InputLayout* m_pLayout;
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;
	};
}