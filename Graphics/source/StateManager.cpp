#include "../includes/StateManager.h"

namespace graphics
{
	StateManager::StateManager()
	{
	}

	StateManager::~StateManager()
	{
	}

	HRESULT StateManager::Initialize()
	{
		HRESULT hr = S_OK;

		graphics::internal::D3D11_DEVICE_HANDLE handle;
		graphics::internal::GetD3D11(&handle);
		m_pDevice4		= handle.pDevice4;
		m_pContext4		= handle.pDeviceContext4;

		if (FAILED(hr)) return hr;

		return S_OK;
	}

	UINT StateManager::CreatePipelineState(StatePipeline* pState, const void* pDescription)
	{
		HRESULT hr = pState->Initialize(m_pDevice4, pDescription);
		if (FAILED(hr)) return UINT_MAX;

		m_settings.push_back(pState);
		return (UINT)m_settings.size() - 1;
	}

	void StateManager::ClearPipelineState(const UINT pipeline)
	{
		m_settings[pipeline]->Clear(m_pContext4);
	}

	void StateManager::UpdatePipelineState(const UINT state, const void* pStateData)
	{
		m_settings[state]->Update(m_pContext4, pStateData);
	}

	void StateManager::SetPipelineState(const UINT state)
	{
		m_settings[state]->SetState(m_pContext4);
	}

	void StateManager::Destroy()
	{
		for (UINT i = 0; i < m_settings.size(); i++)
		{
			m_settings[i]->Delete();
		}

		m_settings.clear();
	}
}