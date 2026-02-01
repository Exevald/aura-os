#include "mm.h"

MemoryManager::MemoryManager(void* start, size_t size) noexcept
	: m_memoryStartAddress(start)
	, m_size(size)
	, m_freeBlocks(nullptr)
{
	if (!start || size < sizeof(BlockHeader))
	{
		return;
	}

	auto startAddr = reinterpret_cast<uintptr_t>(start);
	uintptr_t alignedStart = (startAddr + 7) & ~7;
	size_t alignedSize = size - (alignedStart - startAddr);

	auto* first = reinterpret_cast<BlockHeader*>(alignedStart);
	first->prev = nullptr;
	first->next = nullptr;
	first->SetSize(alignedSize, true);

	m_freeBlocks = first;
}

void* MemoryManager::Allocate(size_t size, size_t align) noexcept
{
	if (size == 0)
	{
		return nullptr;
	}
	if (align < 8)
	{
		align = 8;
	}

	BlockHeader* current = m_freeBlocks;
	while (current != nullptr)
	{
		uintptr_t headerEnd = reinterpret_cast<uintptr_t>(current) + sizeof(BlockHeader);
		uintptr_t alignedData = (headerEnd + align - 1) & ~(align - 1);
		size_t payloadOffset = alignedData - reinterpret_cast<uintptr_t>(current);
		size_t totalRequired = payloadOffset + size;

		totalRequired = (totalRequired + 7) & ~7;

		if (current->GetSize() >= totalRequired)
		{
			BlockHeader* allocatedBlock = current;
			RemoveFromFreeList(allocatedBlock);

			size_t oldTotalSize = allocatedBlock->GetSize();

			if (oldTotalSize > totalRequired + sizeof(BlockHeader) + 8)
			{
				auto* newFreeBlock = reinterpret_cast<BlockHeader*>(reinterpret_cast<uintptr_t>(allocatedBlock) + totalRequired);
				newFreeBlock->SetSize(oldTotalSize - totalRequired, true);
				AddToFreeList(newFreeBlock);
				allocatedBlock->SetSize(totalRequired, false);
			}
			else
			{
				allocatedBlock->SetSize(oldTotalSize, false);
			}

			return reinterpret_cast<void*>(alignedData);
		}
		current = current->next;
	}
	return nullptr;
}

void MemoryManager::Free(void* address) noexcept
{
	if (!address)
	{
		return;
	}

	BlockHeader* block = BlockHeader::GetHeaderFromDataPtr(address);
	block->SetSize(block->GetSize(), true);
	AddToFreeList(block);
	Coalesce(block);
}

void MemoryManager::RemoveFromFreeList(BlockHeader* block)
{
	if (block->prev)
	{
		block->prev->next = block->next;
	}
	else
	{
		m_freeBlocks = block->next;
	}

	if (block->next)
	{
		block->next->prev = block->prev;
	}
}

void MemoryManager::AddToFreeList(BlockHeader* block)
{
	block->next = m_freeBlocks;
	block->prev = nullptr;
	if (m_freeBlocks)
	{
		m_freeBlocks->prev = block;
	}
	m_freeBlocks = block;
}

void MemoryManager::Coalesce(BlockHeader* block)
{
	uintptr_t blockEnd = reinterpret_cast<uintptr_t>(block) + block->GetSize();
	uintptr_t memLimit = reinterpret_cast<uintptr_t>(m_memoryStartAddress) + m_size;

	if (blockEnd < memLimit)
	{
		auto* next = reinterpret_cast<BlockHeader*>(blockEnd);
		if (next->IsFree())
		{
			RemoveFromFreeList(next);
			block->SetSize(block->GetSize() + next->GetSize(), true);
		}
	}

	BlockHeader* cur = m_freeBlocks;
	while (cur != nullptr)
	{
		if (cur != block && cur->IsFree())
		{
			uintptr_t curEnd = reinterpret_cast<uintptr_t>(cur) + cur->GetSize();
			if (curEnd == reinterpret_cast<uintptr_t>(block))
			{
				RemoveFromFreeList(block);
				cur->SetSize(cur->GetSize() + block->GetSize(), true);
				break;
			}
		}
		cur = cur->next;
	}
}