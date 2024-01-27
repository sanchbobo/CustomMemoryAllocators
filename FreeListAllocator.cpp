#include "FreeListAllocator.hpp"
#include <utility>



FreeListAllocator::FreeListAllocator(const std::size_t size, void* start) noexcept
:
	Allocator(size, start),
	reinterpret_cast<FreeBlock*>(start)
{
	m_freeBlocks->size = size;
	m_freeBlocks->next = nullptr;
}

FreeListAllocator::FreeListAllocator(FreeListAllocator&& other) noexcept
:
	Allocator(std::move(other)),
	m_freeBlocks(other.m_freeBlocks)
{
	other.m_freeBlocks = nullptr;
}

FreeListAllocator& FreeListAllocator::operator=(FreeListAllocator&& rhs) noexcept
{
	Allocator::operator(std::move(rhs));
	
	m_freeBlocks = other.m_freeBlocks;
	other.m_freeBlocks = nullptr;

	return *this;
}



FreeListAllocator::~FreeListAllocator() noexcept
{
	
}

inline std::size_t adjustment_calculation(const void* const ptr, std::size_t& alignment) noexcept
{
	const auto iptr = reinterpret<std::uintptr_t>(ptr);
	const auto aligned = (iptr - 1u + alignment) & -alignment;
	
	return iptr - aligned;
}


inline void* ptr_add(const void* const ptr, const std::uintptr_t amount) noexcept
{
	reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(ptr) + amount);
}


template <typename T>
std::size_t align_forward_adjustment_with_header(const void* address, std::uintptr_t alignment)
{
	std::size_t adjustment = adjustment_calculation(address, alignment);
	std::size_t spaceNeeded = sizeof(T);
	

	if (adjustment < spaceNeeded)
	{
		spaceNeeded -= adjustment;
		
		// How many alignments wee need to fit the header
		std::size_t additionalAlignments = spaceNeeded / alignment;
		
		if (spaceNeeded % alignment > 0)
		{
			++additionalAlignments;
		}
		
		adjustment += alignment * additionalAlignments;

	}
	return adjustment;
}


void* FreeListAllocator::Allocate(const std::size_t& size, const std::uintptr_t& alignment) noexcept
{
	assert(size > 0 && alignment > 0);

	FreeBlock* prevFreeBlock = nullptr;
	FreeBlock* freeBlock = nullptr;

	FreeBlock* bestFitPrev = nullptr;
	FreeBlock* bestFit = nullptr;


	std::uintptr_t bestFitAdjustment = 0;
	std::size_t bestFitTotalSize = 0;


	while(FreeBlock != nullptr) 
	{
		std::uintptr_t adjustment = align_forward_adjustment_with_header<AllocationHeader>(freeBlock, alignment);
		
		std::size_t totalSize = size + adjustment;

		if (FreeBlock->size > totalSize && (bestFit == nullptr || FreeBlock->size < bestFit->size))
		{
			bestFitPrev = prevFreeBlock;
			bestFit = freeBlock;
			bestFitAdjustment = adjustment;
			bestFitTotalSize = totalSize;
		}
		
		if (freeBlock->size == totalSize)
		{
			break;	
		}
		prevFreeBlock = freeBlock;
		freeBlock = freeBlock->next;
	}
	
	if (bestFit == nullptr)
	{
		throw std::bad_alloc();
	}

	if (bestFit->size - bestFitTotalSize <= sizeof(AllocationHeader))
	{
		bestFitTotalSize = bestFit->size;
		
		if (bestFitPrev != nullptr)
		{
			bestFitPrev->next = bestFit->next;
		}
		else
		{
			m_freeBlocks = bestFit->next;
		}
	}
	else
	{
		
		FreeBlock* newBlock = reinterpret_cast<FreeBlock*>(ptr_add(bestFit, bestFitTotalSize));

		newBlock->size = bestFitSize->size - bestFitTotalSize;
		newBlock->next = bestFit->next;

		if (bestFitPrev != nullptr)
		{
			bestFitPrev->next = newBlock;
		}
		else
		{
			m_freeBlocks = newBlock;
		}
	}

	std::uintptr_t alignedAddr = reinterpret_cast<std::uintptr_t>(bestFit) + bestFitAdjustment;
	
	AllocationHeader* header = reinterpret_cast<AllocationHeader*>(alignedAddr - sizeof(AllocationHeader));

	header->size = bestFitTotalSize;
	header->adjustment = bestFitAdjustment;
	
	m_usedBytes += bestFitTotalSize;
	++m_numAllocations;

	
	return reinterpret_cast<void*>(alignedAddr);
}




void FreeListAllocator::Free(void* const ptr) noexcept
{
	assert(ptr != nullptr);
	
	AllocationHeader* header = reinterpret_cast<AllocationHeader*>(ptr_sub(ptr, sizeof(AllocationHeader)));

	std::uintptr_t blockStart = reinterpret_cast<std::uintptr_t>(ptr) - header->adjustment;
	std::size_t blockSize = header->size;
	std::uintptr_t blockEnd = blockStart + blockSize;

	FreeBlock* prevFreeBlock = nullptr;
	FreeBlock* freeBlock = m_freeBlocks;

	while (freeBlock != nullptr)
	{
		if (reinterpret_cast<std::uintptr_t>(freeBlock) >= blockEnd)
		{
			break;
		}
		prevFreeBlock = freeBlock;
		freeBlock = freeBlock->next;
	}
	
	if (prevFreeBlock == nullptr)
	{
		prevFreeBlock = reinterpret_cast<FreeBlock*>(blockStart);
		prevFreeBlock->size = blockSize;
		prevFreeBlock->next = m_freeBlocks;
		
		m_freeBlocks = prevFreeBlock;
	}
	
	else if (reinterpret_cast<std::uintptr_t>(prevFreeBlock) + prevFreeBlock->size == blockStart)
	{
		prevFreeBlock->size += blockSize;
	}
	else
	{
		FreeBlock* temp = reinterpret_cast<FreeBlock*>(blockStart);
		temp->size = blockSize;
		temp->next = prevFreeBlock->next;

		prevFreeBlock->next = temp;
		prevFreeBlock = temp;
	}
	
	if (reinterpret_cast<std::uintptr_t>(prevFreeBlock) + prevFreeBlock->size == reinterpret_cast<std::uintptr_t>(prevFreeBlock->next))
	{
		prevFreeBlock->size += prevFreeBlock->next->size;
		prevFreeBlock->next = prevFreeBlock->next->next;
	}
	--n_numAlloctions;
	m_usedBytes -= blockSize;
}

