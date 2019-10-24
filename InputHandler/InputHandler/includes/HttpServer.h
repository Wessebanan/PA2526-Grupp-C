#pragma once
#include <string>
namespace HttpServer
{
	bool GetLocalIp4(std::string& rStringToFill);
	void RunHttpServer();
	void StopHttpServer();
}