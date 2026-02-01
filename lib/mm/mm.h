#pragma once

#include <stddef.h>
#include <stdint.h>

class MemoryManager
{
public:
	struct BlockHeader
	{
		BlockHeader* prev;
		BlockHeader* next;
		size_t size;

		static constexpr size_t FLAG_FREE = 1;

		[[nodiscard]] size_t GetSize() const { return size & ~FLAG_FREE; }
		[[nodiscard]] bool IsFree() const { return (size & FLAG_FREE) != 0; }
		void SetSize(size_t newSize, bool isFree)
		{
			size = newSize | (isFree ? FLAG_FREE : 0);
		}

		static BlockHeader* GetHeaderFromDataPtr(void* dataPtr)
		{
			return reinterpret_cast<BlockHeader*>(reinterpret_cast<uintptr_t>(dataPtr) - sizeof(BlockHeader));
		}
	};

	explicit MemoryManager(void* start, size_t size) noexcept;

	MemoryManager(const MemoryManager&) = delete;
	MemoryManager& operator=(const MemoryManager&) = delete;

	void* Allocate(size_t size, size_t align = 8) noexcept;
	void Free(void* address) noexcept;

private:
	void RemoveFromFreeList(BlockHeader* block);
	void AddToFreeList(BlockHeader* block);
	void Coalesce(BlockHeader* block);

	void* m_memoryStartAddress;
	size_t m_size;
	BlockHeader* m_freeBlocks;
};