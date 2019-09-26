#include "../..//includes/rendering/RenderTechnique.h"
#include "../..//includes/rendering/DefaultTechnique.h"

namespace rendering
{
	static DefaultTechniqueData* gpData		= NULL;

	template<>
	void RenderTechnique::Construct<RENDER_DEFAULT>(
		RENDER_TECHNIQUE_DESC& rDescription)
	{
		rDescription.ByteWidth = sizeof(DefaultTechniqueData);
		rDescription.ppData = (void**)& gpData;
	}

	template<>
	void RenderTechnique::Initialize<RENDER_DEFAULT>(
		graphics::DeviceInterface* pDevice)
	{
	}

	template<>
	void RenderTechnique::Update<RENDER_DEFAULT>(
		graphics::RenderContext* pContext)
	{
	}

	template<>
	void RenderTechnique::Set<RENDER_DEFAULT>(
		graphics::RenderContext* pContext)
	{
	}

	template<>
	void RenderTechnique::Deconstruct<RENDER_DEFAULT>(
		graphics::DeviceInterface* pDevice)
	{
	}

	template<>
	void* RenderTechnique::GetData<RENDER_DEFAULT>()
	{
		return gpData;
	}
}