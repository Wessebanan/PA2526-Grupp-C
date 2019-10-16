#pragma once

/*
	-- Memory Leak Detection --
*/

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>


/*
	-- Win32 API --
*/

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <WinUser.h>

#pragma comment (lib, "Ws2_32.lib")


/*
	-- DirectX --
*/

#include <DirectXMath.h>
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")


/*
	-- Standard Library --
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <vector>
#include <fstream>