#include "../..//includes/rendering/RenderTechnique.h"
#include "../..//includes/rendering/WaterTechnique.h"

namespace rendering
{
	/* 
		initialize	(set static data)	-(Is followed by an upload of static data)
		update		(set dynamic data)	-(Is followed by an upload of dynamic data)
		set & draw	(set state)			-(Is followed by a draw)
	*/

	static WaterTechniqueData* gpData = NULL;

	template<>
	void RenderTechnique::Construct<RENDER_WATER>(
		RENDER_TECHNIQUE_DESC& rDescription)
	{
		rDescription.ByteWidth	= sizeof(WaterTechniqueData);
		rDescription.ppData		= (void**)& gpData;
	}

	template<>
	void RenderTechnique::Initialize<RENDER_WATER>(
		graphics::DeviceInterface* pDevice)
	{
	}

	template<>
	void RenderTechnique::Update<RENDER_WATER>(
		graphics::RenderContext* pContext)
	{
	}

	template<>
	void RenderTechnique::Set<RENDER_WATER>(
		graphics::RenderContext* pContext)
	{
	}

	template<>
	void RenderTechnique::Deconstruct<RENDER_WATER>(
		graphics::DeviceInterface* pDevice)
	{
	}

	template<>
	void* RenderTechnique::GetData<RENDER_WATER>()
	{
		return gpData;
	}
}