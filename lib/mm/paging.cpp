#include "paging.h"
#include "../utils/utils.h"
#include "pmm.h"
#include "vmm.h"
#include "wsclock.h"

extern "C" void loadPageDirectory(uint32_t*);
extern "C" void enablePaging();
extern "C" void flushTLB();

alignas(4096) PageDirectory kernelDirectory;
alignas(4096) PageTable firstPageTable;
alignas(4096) PageTable heapPageTable;

void VirtualMemoryManager::Init()
{
	memset(&kernelDirectory, 0, sizeof(PageDirectory));
	memset(&firstPageTable, 0, sizeof(PageTable));
	memset(&heapPageTable, 0, sizeof(PageTable));
	WSClockManager::Init();

	for (int i = 0; i < 1024; i++)
	{
		firstPageTable.pages[i].frame = i;
		firstPageTable.pages[i].present = 1;
		firstPageTable.pages[i].rw = 1;
	}

	kernelDirectory.tables[0].tableAddr = reinterpret_cast<uintptr_t>(&firstPageTable) >> 12;
	kernelDirectory.tables[0].present = 1;
	kernelDirectory.tables[0].rw = 1;

	kernelDirectory.tables[1].tableAddr = reinterpret_cast<uintptr_t>(&heapPageTable) >> 12;
	kernelDirectory.tables[1].present = 1;
	kernelDirectory.tables[1].rw = 1;

	loadPageDirectory(reinterpret_cast<uint32_t*>(&kernelDirectory));
	enablePaging();
}

bool VirtualMemoryManager::MapPage(const uint32_t virtualAddress)
{
	const uint32_t pdIdx = virtualAddress >> 22;
	const uint32_t ptIdx = (virtualAddress >> 12) & 0x03FF;

	PageTable* targetTable = nullptr;
	if (pdIdx == 0)
	{
		targetTable = &firstPageTable;
	}
	else if (pdIdx == 1)
	{
		targetTable = &heapPageTable;
	}
	else
	{
		return false;
	}

	if (targetTable->pages[ptIdx].present)
	{
		return true;
	}

	uint32_t frame = PMM::AllocateFrame();
	if (frame == (uint32_t)-1)
	{
		PerformPageReplacement();
		frame = PMM::AllocateFrame();
		if (frame == (uint32_t)-1)
		{
			return false;
		}
	}

	targetTable->pages[ptIdx].frame = frame;
	targetTable->pages[ptIdx].present = 1;
	targetTable->pages[ptIdx].rw = 1;

	return true;
}

void VirtualMemoryManager::PerformPageReplacement()
{
	uint32_t victimVA = WSClockManager::FindVictimPage(&kernelDirectory);

	if (victimVA != (uint32_t)-1)
	{
		uint32_t pdIdx = victimVA >> 22;
		uint32_t ptIdx = (victimVA >> 12) & 0x03FF;

		PageTable* table = (pdIdx == 0) ? &firstPageTable : &heapPageTable;
		PTE& page = table->pages[ptIdx];

		PMM::FreeFrame(page.frame);
		page.present = 0;
		flushTLB();
	}
}