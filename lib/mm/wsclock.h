#pragma once

#include "vmm.h"

struct PageMetadata
{
	uint32_t lastAccessTime;
};

class WSClockManager
{
public:
	static void Init();
	static uint32_t FindVictimPage(PageTable* table);

private:
	static PageMetadata m_metadata[1024];
	static uint32_t m_hand;
};