#pragma once
#include <string>
namespace HttpServer
{
	bool GetLocalIp4(std::string& rStringToFill);
	void RunHttpServer(std::string pathToHtml, std::string pathToFavicon);
	void StopHttpServer();
}