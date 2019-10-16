
//#include <d3d11_4.h>
//#include <dxgi1_6.h>
//#include <d3dcompiler.h>
//#include <DirectXMath.h>
//
//#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "dxgi.lib")

//namespace gpu
//{
//	constexpr float GPU_CONSTANT_BUFFER_ALIGNMENT		= 256.0f;
//	constexpr UINT GPU_CONSTANT_BUFFER_BYTES_PER_BIND	= 65536;
//}

#define GPU_CONSTANT_BUFFER_ALIGNMENT 256.0f
#define GPU_CONSTAND_BUFFER_BYTES_PER_BIND 65536

namespace graphics
{
	struct BufferRegion
	{
		UINT Location, Size;
	};

	struct MeshRegion
	{
		// Measured in counts and needs to be converted when used with buffer

		BufferRegion VertexRegion;	
		BufferRegion IndexRegion;
	};
}