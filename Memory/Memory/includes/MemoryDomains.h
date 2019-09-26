#pragma once
#include "MemoryGlobals.h"

/*
	-- HOW TO CREATE A CUSTOM DOMAIN --
	
	1. Create an entry in MemoryDomains::
	2. Create an entry in MEMORY_DOMAIN_SIZES
*/

enum MemoryDomains
{
	ECS,
	Misc,

	MemoryDomainCount,
	Undefined
};


const std::map<MemoryDomain, DomainInfo> MEMORY_DOMAIN_SIZES
{
	/*
		Format:
		MEMORY_DOMAIN( <Domain name>, <HeapType>, <size in bytes> )

		Available heap types:
			- Linear
	*/

	MEMORY_DOMAIN(ECS, Linear, 100),
	MEMORY_DOMAIN(Misc, Linear, 100),
};