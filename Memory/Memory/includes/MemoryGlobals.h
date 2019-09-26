#pragma once

#include <map>
#include <string>

#define DEBUG_CONSOLE_LINE(line) std::cout << line << std::endl;
#define DEBUG_CONSOLE(str) std::cout << str;

using uint = unsigned int;

using MemoryDomain = unsigned int;

using HeapType = unsigned int;
enum HeapTypes
{
	Linear
};

struct DomainInfo
{
	MemoryDomain domain;
	HeapType type;
	uint size;
	std::string domainString;
};
#define MEMORY_DOMAIN(domain, type, size) { domain, { domain, type, size, #domain }}