#pragma once

using uint = unsigned int;

#define DENY_COPY(ClassName) ClassName(const ClassName& other) = delete; ClassName& operator=(const ClassName& other) = delete;