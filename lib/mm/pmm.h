#pragma once

#include <stddef.h>
#include <stdint.h>

class PMM
{
public:
	static void Init(uint32_t kernelEndAddress);
	static uint32_t AllocateFrame();
	static void FreeFrame(int frameIndex);

	static constexpr uint32_t PAGE_SIZE = 4096;
	static constexpr uint32_t RAM_SIZE = 128 * 1024 * 1024;
	static constexpr uint32_t FRAMES_COUNT = RAM_SIZE / PAGE_SIZE;
	static constexpr uint32_t BITMAP_SIZE = FRAMES_COUNT / 32;

private:
	static uint32_t m_framesBitmap[BITMAP_SIZE];
};
