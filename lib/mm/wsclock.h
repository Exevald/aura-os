#pragma once

#include "vmm.h"
#include <stdint.h>

struct PageMetadata
{
	uint32_t lastAccessTime;
};

class WSClockManager
{
public:
	static void Init();
	static uint32_t FindVictimPage(PageDirectory* dir);

private:
	static PageMetadata m_metadata[2048];
	static uint32_t m_globalHand;
};