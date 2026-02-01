#include "pmm.h"
#include "../utils/utils.h"

uint32_t PMM::m_framesBitmap[BITMAP_SIZE];

void PMM::Init(const uint32_t kernelEndAddress)
{
	memset(m_framesBitmap, 0, sizeof(m_framesBitmap));

	const uint32_t reserved_frames = (kernelEndAddress + PAGE_SIZE - 1) / PAGE_SIZE;
	for (uint32_t i = 0; i < reserved_frames; i++)
	{
		const uint32_t wordIndex = i / 32;
		const uint32_t bitIndex = i % 32;
		m_framesBitmap[wordIndex] |= (1 << bitIndex);
	}
}

uint32_t PMM::AllocateFrame()
{
	for (uint32_t i = 0; i < BITMAP_SIZE; i++)
	{
		if (m_framesBitmap[i] != 0xFFFFFFFF)
		{
			for (int j = 0; j < 32; j++)
			{
				if (!(m_framesBitmap[i] & (1 << j)))
				{
					m_framesBitmap[i] |= (1 << j);
					return i * 32 + j;
				}
			}
		}
	}

	return -1;
}

void PMM::FreeFrame(const int frameIndex)
{
	const uint32_t wordIndex = frameIndex / 32;
	const uint32_t bitIndex = frameIndex % 32;
	m_framesBitmap[wordIndex] &= ~(1 << bitIndex);
}