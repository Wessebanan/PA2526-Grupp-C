#pragma once
#include "DrawManager.h"

#include <iostream>

namespace rendering
{
	struct RENDER_TECHNIQUE_DESC
	{
		UINT ByteWidth;
		void** ppData;
	};

	class RenderTechnique
	{
	public:
		/*
			Construct the techniques
		*/
		template <RENDER_TECHNIQUES T>
		static void Construct(RENDER_TECHNIQUE_DESC& rDescription);

		/* 
			Set data that is to be static
		*/
		template <RENDER_TECHNIQUES T>
		static void Initialize(graphics::DeviceInterface* pDevice);

		/*
			Set data that is to be dynamic
		*/
		template <RENDER_TECHNIQUES T>
		static void Update(graphics::RenderContext* pContext);

		/*
			Set state to be defined when rendering
		*/
		template <RENDER_TECHNIQUES T>
		static void Set(graphics::RenderContext* pContext);

		/*
			Destruct the techniques
		*/
		template <RENDER_TECHNIQUES T>
		static void Deconstruct(graphics::DeviceInterface* pDevice);

		/*
			Get Data specific to a technique
		*/
		template <RENDER_TECHNIQUES T>
		static void* GetData();

	private:
	};

	namespace ClassMethodLooper 
	{
		template<int N, template<int I> class Wrapper, typename Parent, typename ... Args>
		struct Loop 
		{
			static void impl(Parent* parent, const Args& ... args) 
			{
				//Build Wrapper struct.
				Wrapper<N> member{};


				//Call operator() of Wrapper
				member(parent, args...);

				//Continue Loop
				Loop<N - 1, Wrapper, Parent, Args ...>::impl(parent, args...);
			}
		};

		template<template<int I> class Wrapper, typename Parent, typename ... Args>
		struct Loop<0, Wrapper, Parent, Args ...> 
		{
			static void impl(Parent* parent, const Args& ... args) 
			{
				//Do Nothing (terminate loop)
			}
		};
	}

	//Call this to start the loop.
	template<int N, template<int I> class Wrapper, typename Parent, typename ... Args>
	void ClassMethodLoop(Parent* parent, const Args& ... args) 
	{
		ClassMethodLooper::Loop<N, Wrapper, Parent, Args ...>::impl(parent, args...);
	}

	// Used for inverting to (0 - 10) instead of (10 - 0)
#define INDEX (RENDER_TECHNIQUES_COUNT - N)

	class RenderTechniques {
	public:
		void SetDrawManager(DrawManager* pDrawManager)
		{
			m_pDrawManager = pDrawManager;
		}

		void ConstructAll(RENDER_TECHNIQUE_DESC desc[RENDER_TECHNIQUES_COUNT])
		{
			ClassMethodLoop<RENDER_TECHNIQUES_COUNT, ConstructTechniques>(this, desc);
		}

		void InitializeAll(graphics::DeviceInterface* pDevice) 
		{
			ClassMethodLoop<RENDER_TECHNIQUES_COUNT, InitializeTechniques>(this, pDevice);
		}

		void UpdateAll(graphics::RenderContext* pContext)
		{
			ClassMethodLoop<RENDER_TECHNIQUES_COUNT, UpdateTechniques>(this, pContext);
		}

		void SetAndDrawAll(graphics::RenderContext* pContext)
		{
			m_pDrawManager->Reset(pContext);
			ClassMethodLoop<RENDER_TECHNIQUES_COUNT, SetTechniques>(this, pContext);
		}

		void SetAndDrawAllWithoutPS(graphics::RenderContext* pContext)
		{
			m_pDrawManager->Reset(pContext);
			ClassMethodLoop<RENDER_TECHNIQUES_COUNT, SetTechniquesWithoutPS>(this, pContext);
		}

		void DeconstructAll(graphics::DeviceInterface* pDevice)
		{
			ClassMethodLoop<RENDER_TECHNIQUES_COUNT, DeconstructTechniques>(this, pDevice);
		}

		template<RENDER_TECHNIQUES T>
		void* GetData()
		{
			return m_technique.GetData<T>();
		}

	private:
		RenderTechnique m_technique;
		DrawManager* m_pDrawManager;

		template<int N>
		struct ConstructTechniques
		{
			void operator()(
				RenderTechniques* parent, 
				RENDER_TECHNIQUE_DESC desc[RENDER_TECHNIQUES_COUNT]) const
			{
				parent->Construct<N>(desc[INDEX]);
			}
		};

		template<int N>
		struct InitializeTechniques 
		{
			void operator()(
				RenderTechniques* parent, 
				graphics::DeviceInterface* pDevice) const
			{
				parent->Initialize<N>(pDevice);
			}
		};

		template<int N>
		struct UpdateTechniques 
		{
			void operator()(
				RenderTechniques* parent, 
				graphics::RenderContext* pContext) const
			{
				parent->Update<N>(pContext);
			}
		};

		template<int N>
		struct SetTechniques
		{
			void operator()(
				RenderTechniques* parent, 
				graphics::RenderContext* pContext) const
			{
				parent->Set<N>(pContext);
			}
		};

		template<int N>
		struct SetTechniquesWithoutPS
		{
			void operator()(
				RenderTechniques* parent,
				graphics::RenderContext* pContext) const
			{
				parent->SetWithoutPS<N>(pContext);
			}
		};

		template<int N>
		struct DeconstructTechniques
		{
			void operator()(
				RenderTechniques* parent,
				graphics::DeviceInterface* pDevice) const
			{
				parent->Deconstruct<N>(pDevice);
			}
		};

		template<int N>
		void Construct(RENDER_TECHNIQUE_DESC& desc)
		{
			RenderTechnique::Construct<RENDER_TECHNIQUES(INDEX)>(desc);
		}

		template<int N>
		void Initialize(graphics::DeviceInterface* pDevice)
		{
			RenderTechnique::Initialize<RENDER_TECHNIQUES(INDEX)>(pDevice);
		}

		template<int N>
		void Update(graphics::RenderContext* pContext)
		{
			RenderTechnique::Update<RENDER_TECHNIQUES(INDEX)>(pContext);
		}

		template<int N>
		void Set(graphics::RenderContext* pContext)
		{
			RenderTechnique::Set<RENDER_TECHNIQUES(INDEX)>(pContext);

			m_pDrawManager->Draw(RENDER_TECHNIQUES(INDEX), pContext);
		}

		template<int N>
		void SetWithoutPS(graphics::RenderContext* pContext)
		{
			RenderTechnique::Set<RENDER_TECHNIQUES(INDEX)>(pContext);

			pContext->DiscardPixelShader();
			m_pDrawManager->Draw(RENDER_TECHNIQUES(INDEX), pContext);
		}

		template<int N>
		void Deconstruct(graphics::DeviceInterface* pDevice)
		{
			RenderTechnique::Deconstruct<RENDER_TECHNIQUES(INDEX)>(pDevice);
		}
	};
}

