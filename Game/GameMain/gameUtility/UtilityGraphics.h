#pragma once
#include <string>
static inline uint32_t PACK(uint8_t c0, uint8_t c1, uint8_t c2, uint8_t c3) {
	return ((uint32_t)c0 << 24) | ((uint32_t)c1 << 16) | ((uint32_t)c2 << 8) | (uint32_t)c3;
}

static inline const std::string GetShaderFilepath(const char* pFilename)
{
	std::string filepath = "..//";

#ifdef _DEBUG
	filepath.append("shaders_d//");
#else
	filepath.append("shaders//");
#endif // _DEBUG
	filepath.append(pFilename);

	return filepath;
}