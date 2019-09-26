#pragma once

#include "..//graphics/DeviceInterface.h"

#ifdef _DEBUG
#pragma comment(lib, "GraphicsEngine_d.lib")
#else
#pragma comment(lib, "GraphicsEngine.lib")
#endif // DEBUG

namespace rendering
{
	enum RENDER_TECHNIQUES {
		RENDER_DEFAULT,
		RENDER_WATER,

		RENDER_TECHNIQUES_COUNT,
	};
}