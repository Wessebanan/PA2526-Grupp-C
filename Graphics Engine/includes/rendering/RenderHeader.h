#pragma once

#include "..//graphics/DeviceInterface.h"

#ifdef _DEBUG
#pragma comment(lib, "GraphicsEngine_d.lib")
#else
#pragma comment(lib, "GraphicsEngine.lib")
#endif // DEBUG

inline uint32_t PACK(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3) {
	return (c0 << 24) | (c1 << 16) | (c2 << 8) | c3;
}

namespace rendering
{
	enum RENDER_TECHNIQUES 
	{
		RENDER_DEFAULT,
		RENDER_TRANSFORMATION,
		 
		RENDER_TECHNIQUES_COUNT,
	};
} 