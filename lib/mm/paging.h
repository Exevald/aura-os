#pragma once

#include <stdint.h>

class VirtualMemoryManager
{
public:
	static void Init();
	static bool MapPage(uint32_t virtualAddress);

private:
	static void PerformPageReplacement();
};