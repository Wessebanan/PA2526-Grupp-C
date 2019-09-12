#pragma once

#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include <string>
#include <array>


enum INPUT_LAYOUT_PARAM
{
	INPUT_LAYOUT_PARAM_POSITION = 0,
	INPUT_LAYOUT_PARAM_NORMAL = 1,
	INPUT_LAYOUT_PARAM_UV = 2,
};

struct GraphicsInterface
{
	UINT id;
};
