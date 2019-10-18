#pragma once

#pragma once

#pragma once

#include "StateSettings.h"

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
		UINT CreateState(StateSettings* pState, const void* pDescription);

		/*
			Update pipeline data (Every pipeline has their own structs for data)
		*/
		void UpdateState(const UINT state, const void* pStateData);

		/*
			Execute desired pipeline (will use all created programs)
		*/
		void SetState(const UINT state);

		void Destroy();
	private:
		ID3D11Device4* m_pDevice4;
		ID3D11DeviceContext4* m_pContext4;

		IDXGIFactory6* m_pFactory6;
		IDXGIAdapter4* m_pAdapter4;

		std::vector<StateSettings*> m_settings;
	};
}