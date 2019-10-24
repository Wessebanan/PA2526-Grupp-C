#pragma once
#include <string>

bool GetLocalIp4(std::string& rStringToFill);
void RunHttpServer();
void StopHttpServer();