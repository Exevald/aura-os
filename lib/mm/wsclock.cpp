#include "wsclock.h"
#include "../utils/timer.h"

PageMetadata WSClockManager::m_metadata[2048];
uint32_t WSClockManager::m_globalHand = 0;
constexpr uint32_t TAU = 50;

void WSClockManager::Init()
{
	m_globalHand = 0;
	for (auto& i : m_metadata)
	{
		i.lastAccessTime = 0;
	}
}

uint32_t WSClockManager::FindVictimPage(PageDirectory* dir)
{
	uint32_t startHand = m_globalHand;
	uint32_t currentTimestamp = Timer::GetTime();

	do
	{
		uint32_t tableIdx = m_globalHand / 1024;
		uint32_t pageIdx = m_globalHand % 1024;

		if (dir->tables[tableIdx].present)
		{
			auto* table = (PageTable*)(dir->tables[tableIdx].tableAddr << 12);
			PTE& pte = table->pages[pageIdx];

			bool isKernelCore = (tableIdx == 0 && pageIdx < 256);

			if (pte.present && !isKernelCore)
			{
				if (pte.accessed)
				{
					pte.accessed = 0;
					m_metadata[m_globalHand].lastAccessTime = currentTimestamp;
				}
				else
				{
					uint32_t age = currentTimestamp - m_metadata[m_globalHand].lastAccessTime;
					if (age > TAU)
					{
						if (!pte.dirty)
						{
							uint32_t victimVirtualAddress = m_globalHand * 4096;

							m_globalHand = (m_globalHand + 1) % 2048;
							return victimVirtualAddress;
						}
						else
						{
							pte.dirty = 0;
						}
					}
				}
			}
		}
		m_globalHand = (m_globalHand + 1) % 2048;
	} while (m_globalHand != startHand);

	return (uint32_t)-1;
}