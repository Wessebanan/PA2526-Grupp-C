#include "ConnectionHelper.h"
#include <winsock2.h>
#include <iphlpapi.h>
#include <iostream>
#pragma comment(lib, "IPHLPAPI.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

bool GetLocalIp4(std::string& stringToFill)
{
	PIP_ADAPTER_INFO pAdapterInfo;
	DWORD dwRetVal = 0;
	UINT i;

	// Allocate how much memory we think is necessary
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == nullptr)
	{
		std::cout << "Error when fetching IP: Error allocating memory!\n";
		return false;
	}
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		// If we guessed wrong, resize
		FREE(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(ulOutBufLen);
		if (pAdapterInfo == nullptr)
		{
			std::cout << "Error when fetching IP: Error allocating memory (First call)!\n";
			return false;
		}
	}

	// Make sure we don't get any errors
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) != NO_ERROR) {
		std::cout << "Error when fetching IP: Error getting adapters info (Second call)!\n"
			<< dwRetVal << std::endl;
		return false;
	}
	// Fill the string with the IP adress
	stringToFill.assign(pAdapterInfo->IpAddressList.IpAddress.String);

	// Free memory
	if (pAdapterInfo)
		FREE(pAdapterInfo);

	return true;
}
