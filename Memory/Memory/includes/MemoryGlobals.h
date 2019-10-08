#pragma once

using uint = unsigned int;

enum HeapType
{
	HEAP_TYPE_LINEAR,
};

/*
	This macro will delete the copy constructor and copy operator, making it impossible to copy the object.
	If this isn't done, the class/struct will be given default copy functionality by the c++ compiler.
*/
#define DENY_COPY(ClassName) ClassName(const ClassName& other) = delete; ClassName& operator=(const ClassName& other) = delete;