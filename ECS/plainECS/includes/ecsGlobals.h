#pragma once

#include "nameOf.h"

typedef unsigned int ID;
typedef unsigned int TypeID;

enum MEMORY_DESC_TYPE { MEMORY_DESC_TYPE_UNKOWN, MEMORY_DESC_TYPE_SIZE, MEMORY_DESC_TYPE_COUNT };

typedef char COMP_FLAG;
enum COMP_FLAGS
{
	COMP_FLAG_ALIVE = 1,
	COMP_FLAG_VISIBLE = 2,
};