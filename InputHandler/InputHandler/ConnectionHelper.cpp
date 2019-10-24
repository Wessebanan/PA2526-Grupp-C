#include "ConnectionHelper.h"
#define NOMINMAX
#include <winsock2.h>
#include <iphlpapi.h>
#include <iostream>
#include "httplib.h"
using namespace httplib;
#pragma comment(lib, "IPHLPAPI.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

std::thread* gServerThread = nullptr;
Server gServer;

bool GetLocalIp4(std::string& rStringToFill)
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

void RunHttpServer()
{
	if (!gServer.is_valid()) {
		std::cout << "The server has an error...\n";
		return;
	}
	gServerThread = new std::thread(HttpServerThread);
}

void StopHttpServer()
{
	gServer.stop();
	gServerThread->join();
	delete gServerThread;
}

#pragma region HELLO
std::string dump_headers(const Headers& headers) {
	std::string s;
	char buf[BUFSIZ];

	for (auto it = headers.begin(); it != headers.end(); ++it) {
		const auto& x = *it;
		snprintf(buf, sizeof(buf), "%s: %s\n", x.first.c_str(), x.second.c_str());
		s += buf;
	}

	return s;
}

std::string log(const Request& req, const Response& res) {
	std::string s;
	char buf[BUFSIZ];

	s += "================================\n";

	snprintf(buf, sizeof(buf), "%s %s %s", req.method.c_str(),
		req.version.c_str(), req.path.c_str());
	s += buf;

	std::string query;
	for (auto it = req.params.begin(); it != req.params.end(); ++it) {
		const auto& x = *it;
		snprintf(buf, sizeof(buf), "%c%s=%s",
			(it == req.params.begin()) ? '?' : '&', x.first.c_str(),
			x.second.c_str());
		query += buf;
	}
	snprintf(buf, sizeof(buf), "%s\n", query.c_str());
	s += buf;

	s += dump_headers(req.headers);

	s += "--------------------------------\n";

	snprintf(buf, sizeof(buf), "%d %s\n", res.status, res.version.c_str());
	s += buf;
	s += dump_headers(res.headers);
	s += "\n";

	if (!res.body.empty()) { s += res.body; }

	s += "\n";

	return s;
}
#pragma endregion

void HttpServerThread()
{
	gServer.Get("/", [=](const Request& /*req*/, Response& res) {
		std::string buf;
		if (!GetContentAsString("mobileSite.html", buf))
		{
			buf = "<p>Error: <span style='color:red;'>mobileSite.html could not be found</span></p>";
		}
		res.set_content(buf, "text/html");
		//res.set_redirect("/hi");
	});

	gServer.Get("/favicon.ico", [=](const Request& /*req*/, Response& res) {
		std::string buf;
		if (!GetContentAsString("favicon.ico", buf, true))
		{
			buf = "<p>Error 404: <span style='color:red;'>favicon.ico could not be found</span></p>";
			res.set_content(buf, "text/html");
		}
		else
		{
			res.set_content(buf, "image/x-icon");
		}
	});

	//svr.Get("/hi", [](const Request& /*req*/, Response& res) {
	//	res.set_content("Hello World!\n", "text/plain");
	//});

	gServer.set_error_handler([](const Request& /*req*/, Response& res) {
		const char* fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
		char buf[BUFSIZ];
		snprintf(buf, sizeof(buf), fmt, res.status);
		res.set_content(buf, "text/html");
	});

	gServer.set_logger([](const Request& req, const Response& res) {
		printf("%s", log(req, res).c_str());
	});

	std::string ip;
	GetLocalIp4(ip);

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
