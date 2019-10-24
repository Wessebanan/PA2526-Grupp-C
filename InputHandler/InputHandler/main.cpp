#include "ConnectionHelper.h"
#include <Windows.h>
#include <iostream>

int main()
{
	RunHttpServer();
	std::string ip;
	GetLocalIp4(ip);
	std::cout << "Server open at adress: " << ip << std::endl;
	Sleep(20000);
	StopHttpServer();
	return 0;
}