#pragma once

#include "RenderHeader.h"

inline uint32_t PACK(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3) {
	return (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;
}

namespace rendering
{
	struct DefaultTechniqueData
	{
		graphics::GraphicsPipeline pipeline;
	};
}