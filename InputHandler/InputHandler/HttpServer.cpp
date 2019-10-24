#include "HttpServer.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <iphlpapi.h>
#include <iostream>
#include "httplib.h"
using namespace httplib;
#pragma comment(lib, "IPHLPAPI.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

bool HttpServer::GetLocalIp4(std::string& rStringToFill)
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
	rStringToFill.assign(p_adapter_info->IpAddressList.IpAddress.String);

	// Free memory
	if (p_adapter_info)
		FREE(p_adapter_info);

	return true;
}

std::thread* gServerThread = nullptr;
Server gServer;
std::string gPathToHtml;
std::string gPathToFavicon;

void HttpServerThread();
bool GetContentAsString(std::string rPath, std::string& rStringToFill, bool binary);

void HttpServer::RunHttpServer(std::string pathToHtml, std::string pathToFavicon)
{
	if (!gServer.is_valid()) {
		std::cout << "The server has an error...\n";
		return;
	}
	gPathToHtml = pathToHtml;
	gPathToFavicon = pathToFavicon;
	gServerThread = new std::thread(HttpServerThread);
}

void HttpServer::StopHttpServer()
{
	gServer.stop();
	gServerThread->join();
	delete gServerThread;
}

#ifdef _DEBUG
#pragma region Logging
std::string dump_headers(const Headers& rHeaders) {
	std::string s;
	char buf[BUFSIZ];

	for (auto it = rHeaders.begin(); it != rHeaders.end(); ++it) {
		const auto& x = *it;
		snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
		s += buf;
	}

	return s;
}

std::string log(const Request& rRequest, const Response& rResponse) {
	std::string s;
	char buf[BUFSIZ];

	s += "================================\n";

	snprintf(buf, sizeof(buf), "%s %s %s", rRequest.method.c_str(),
		rRequest.version.c_str(), rRequest.path.c_str());
	s += buf;

	std::string query;
	for (auto it = rRequest.params.begin(); it != rRequest.params.end(); ++it) {
		const auto& x = *it;
		snprintf(buf, sizeof(buf), "%c%s=%s",
			(it == rRequest.params.begin()) ? '?' : '&', x.first.c_str(),
			x.second.c_str());
		query += buf;
	}
	snprintf(buf, sizeof(buf), "%s\n", query.c_str());
	s += buf;

	s += dump_headers(rRequest.headers);

	s += "--------------------------------\n";

	snprintf(buf, sizeof(buf), "%d %s\n", rResponse.status, rResponse.version.c_str());
	s += buf;
	s += dump_headers(rResponse.headers);
	s += "\n";

	if (!rResponse.body.empty()) {
		s += rResponse.body;
	}

	s += "\n";

	return s;
}
#pragma endregion
#endif

void HttpServerThread()
{
	gServer.Get("/", [=](const Request& /*rRequest*/, Response& rResponse)
	{
		std::string buf;
		if (!GetContentAsString(gPathToHtml, buf, false))
		{
			buf = "<p>Error: <span style='color:red;'>mobileSite.html could not be found</span></p>";
			rResponse.set_content(buf, "text/html");
		}
		else
		{
			std::string ip;
			HttpServer::GetLocalIp4(ip);
			buf.insert(0, "<script>var ip_address=\"" + ip + "\";</script>\n");
			rResponse.set_content(buf, "text/html");
		}
	});

	gServer.Get("/favicon.ico", [=](const Request& /*rRequest*/, Response& rResponse)
	{
		std::string buf;
		if (!GetContentAsString(gPathToFavicon, buf, true))
		{
			buf = "<p>Error 404: <span style='color:red;'>favicon.ico could not be found</span></p>";
			rResponse.set_content(buf, "text/html");
		}
		else
		{
			rResponse.set_content(buf, "image/x-icon");
		}
	});

	gServer.set_error_handler([](const Request& /*rRequest*/, Response& rResponse)
	{
		const char* p_format = "<p>Error Status: <span style='color:red;'>%d</span></p>";
		char buf[BUFSIZ];
		snprintf(buf, sizeof(buf), p_format, rResponse.status);
		rResponse.set_content(buf, "text/html");
	});

#ifdef _DEBUG
	gServer.set_logger([](const Request& rRequest, const Response& rResponse)
	{
		printf("%s", log(rRequest, rResponse).c_str());
	});
#endif

	std::string ip;
	HttpServer::GetLocalIp4(ip);

	gServer.listen(ip.c_str(), 80);
}

bool GetContentAsString(std::string path, std::string& rStringToFill, bool binary)
{
	std::FILE* p_file;
	if (binary)
	{
		p_file = std::fopen(path.c_str(), "r");
	}
	else
	{
		p_file = std::fopen(path.c_str(), "rb");
	}
	if (p_file)
	{
		std::fseek(p_file, 0, SEEK_END);
		size_t len = std::ftell(p_file);
		std::fseek(p_file, 0, SEEK_SET);
		//std::string contents(len + 1, '\0');
		//if (binary)
		//{
		//	rStringToFill.reserve(len);
		//}
		//else
		//{
			rStringToFill.resize(len + 1, '\0');
		//}
		std::fread(&rStringToFill[0], 1, len, p_file);
		fclose(p_file);
	}
	else
	{
		return false;
	}
	return true;
}
