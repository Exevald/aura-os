#include "heap.h"
#include "mm.h"
#include "paging.h"

#include <stddef.h>

MemoryManager* kernelMemoryManager = nullptr;

static constexpr uintptr_t HEAP_START = 0x400000;
static constexpr size_t HEAP_SIZE = 1024 * 1024;

static char kmmBuffer[sizeof(MemoryManager)];

void* operator new(size_t, void* p) noexcept { return p; }

void heap::InitKernelHeap()
{
	for (uintptr_t addr = HEAP_START; addr < HEAP_START + HEAP_SIZE; addr += 4096)
	{
		VirtualMemoryManager::MapPage(addr);
	}

	kernelMemoryManager = new (kmmBuffer) MemoryManager((void*)HEAP_START, HEAP_SIZE);
}

extern "C" void* malloc(size_t size)
{
	return kernelMemoryManager ? kernelMemoryManager->Allocate(size) : nullptr;
}

extern "C" void free(void* ptr)
{
	if (kernelMemoryManager)
	{
		kernelMemoryManager->Free(ptr);
	}
}

void* operator new(size_t size) { return malloc(size); }
void* operator new[](size_t size) { return malloc(size); }
void operator delete(void* p) noexcept { free(p); }
void operator delete(void* p, size_t) noexcept { free(p); }
void operator delete[](void* p) noexcept { free(p); }
void operator delete[](void* p, size_t) noexcept { free(p); }
