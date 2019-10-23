#include "pch.h"
#include "ConnectionHelper.h"

TEST(TestCaseName, TestName) {
	// Unfortunately, this is a test someone has to verify themselves
	std::string ip;
	EXPECT_TRUE(GetLocalIp4(ip));
	std::cout << "Your local IP should be: " << ip
		<<"\nCheck with ipconfig\n";
}