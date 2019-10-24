#pragma once
#include <string>

bool GetLocalIp4(std::string& rStringToFill);
void RunHttpServer();
void StopHttpServer();
void HttpServerThread();
bool GetContentAsString(std::string rPath, std::string& rStringToFill, bool binary = false);