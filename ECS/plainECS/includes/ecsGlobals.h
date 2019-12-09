#pragma once

#include "nameOf.h"

typedef unsigned int ID;
typedef unsigned int TypeID;

enum MEMORY_DESC_TYPE { MEMORY_DESC_TYPE_UNKOWN, MEMORY_DESC_TYPE_SIZE, MEMORY_DESC_TYPE_COUNT };

typedef int STATE_FLAGS;
enum STATE_FLAG
{
	STATE_FLAG_ALIVE = 1,
	STATE_FLAG_VISIBLE = 2,
};