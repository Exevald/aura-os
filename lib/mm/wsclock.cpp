#include "wsclock.h"
#include "../utils/timer.h"

PageMetadata WSClockManager::m_metadata[1024];
uint32_t WSClockManager::m_hand = 0;
constexpr uint32_t TAU = 50;

void WSClockManager::Init()
{
	m_hand = 0;
	for (auto& [lastAccessTime] : m_metadata)
	{
		lastAccessTime = 0;
	}
}

uint32_t WSClockManager::FindVictimPage(PageTable* table)
{
	const uint32_t startHand = m_hand;
	const uint32_t currentTimestamp = Timer::GetTime();

	do
	{
		if (PTE& pte = table->pages[m_hand]; pte.present)
		{
			if (pte.accessed)
			{
				pte.accessed = 0;
				m_metadata[m_hand].lastAccessTime = currentTimestamp;
			}
			else
			{
				if (const uint32_t age = currentTimestamp - m_metadata[m_hand].lastAccessTime; age > TAU)
				{
					if (pte.dirty)
					{
						pte.dirty = 0;
						m_metadata[m_hand].lastAccessTime = currentTimestamp;
					}
					else
					{
						const auto victim = m_hand;
						m_hand++;
						if (m_hand >= 1024)
						{
							m_hand = 0;
						}
						return victim;
					}
				}
			}
		}

		m_hand++;
		if (m_hand >= 1024)
		{
			m_hand = 0;
		}
	} while (m_hand != startHand);

	return -1;
}