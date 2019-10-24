#pragma once

#include "RenderContext.h"

namespace graphics
{
	class StateSettings
	{
	public:
		StateSettings() {}
		virtual ~StateSettings() {}

		virtual HRESULT Initialize(
			ID3D11Device4* pDevice4, 
			ID3D11DeviceContext4* pContext4, 
			const void* pDescription) = 0;
		
		virtual void Update(
			ID3D11Device4* pDevice4, 
			ID3D11DeviceContext4* pContext4, 
			const void* pStateData) = 0;
		
		virtual void Set(
			ID3D11Device4* pDevice4, 
			ID3D11DeviceContext4* pContext4) = 0;

		virtual void Destroy() = 0;

		void Delete() { this->Destroy(); delete this; }
	private:

	};
}