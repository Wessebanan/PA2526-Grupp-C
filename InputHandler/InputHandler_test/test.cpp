#include "pch.h"
#include "HttpServer.h"

int main(int argc, char** argv)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(HttpServer, GetLocalIP) {
	// Unfortunately, this is a test someone has to verify themselves
	std::string ip;
	EXPECT_TRUE(HttpServer::GetLocalIp4(ip));
	std::cout << "Your local IP should be: " << ip
		<<"\nCheck with ipconfig\n";
}

TEST(HttpServer, HostServer)
{
	HttpServer::RunHttpServer("../InputHandler/mobileSite.html","../InputHandler/favicon.ico");
	std::string ip;
	HttpServer::GetLocalIp4(ip);
	std::cout << "Server open for 20 sec at adress: " << ip << std::endl;
	Sleep(20000);
	HttpServer::StopHttpServer();
}