#include "ConnectionHelper.h"
#include <winsock2.h>
#include <iphlpapi.h>
#include <iostream>
#pragma comment(lib, "IPHLPAPI.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

bool GetLocalIp4(std::string& stringToFill)
{
	PIP_ADAPTER_INFO p_adapter_info;
	DWORD ret_val = 0;
	UINT i;

	// Allocate how much memory we think is necessary
	ULONG out_buf_len = sizeof(IP_ADAPTER_INFO);
	p_adapter_info = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (p_adapter_info == nullptr)
	{
		std::cout << "Error when fetching IP: Error allocating memory!\n";
		return false;
	}
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(p_adapter_info, &out_buf_len) == ERROR_BUFFER_OVERFLOW)
	{
		// If we guessed wrong, resize
		FREE(p_adapter_info);
		p_adapter_info = (IP_ADAPTER_INFO*)MALLOC(out_buf_len);
		if (p_adapter_info == nullptr)
		{
			std::cout << "Error when fetching IP: Error allocating memory (First call)!\n";
			return false;
		}
	}

	// Make sure we don't get any errors
	if ((ret_val = GetAdaptersInfo(p_adapter_info, &out_buf_len)) != NO_ERROR) {
		std::cout << "Error when fetching IP: Error getting adapters info (Second call)!\n"
			<< ret_val << std::endl;
		return false;
	}
	// Fill the string with the IP adress
	stringToFill.assign(p_adapter_info->IpAddressList.IpAddress.String);

	// Free memory
	if (p_adapter_info)
		FREE(p_adapter_info);

	return true;
}
