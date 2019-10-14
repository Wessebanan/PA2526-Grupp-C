#pragma once

#include "RenderContext.h"
#include "RenderManager.h"

namespace graphics
{
	class Pipeline
	{
	public:
		virtual ~Pipeline() {}

		virtual void Initialize(ID3D11Device4* pDevice4, const void* pDescription) = 0;
		virtual void Update(ID3D11DeviceContext4* pContext4, const void* pData) = 0;
		
		void Delete() { this->Destroy(); delete this; }

	private:
		virtual void Destroy() = 0;

	};

	class GraphicsPipeline : public Pipeline
	{
	public:
		virtual void PreExecute(
			ID3D11DeviceContext4* pContext4, 
			ID3D11VertexShader* pVertexShader,
			ID3D11PixelShader* pPixelShader) = 0;
		
		virtual void Begin(ID3D11DeviceContext4* pContext4) = 0;
		virtual void End(ID3D11DeviceContext4* pContext4, RenderManagerData* pData) = 0;
	};

	class ComputePipeline : public Pipeline
	{
	public:
		virtual void Execute(
			ID3D11DeviceContext4* pContext4,
			ID3D11ComputeShader* pComputeShader) = 0;
	};
}