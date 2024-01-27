#include "Allocator.hpp"
#include <cassert>


struct FreeBlock;

class FreeListAllocator : public Allocator
{
protected:
	FreeBlock* m_freeBlocks;


public:

	FreeListAllocator(std::size_t size, void* const start) noexcept;
	
	FreeListAllocator(FreeListAllocator&) = delete;
	FreeListAllocator& operator=(FreeListAllocator&) = delete;
	
	FreeListAllocator(FreeListAllocator&&) noexcept;

	FreeListAllocator& operator=(FreeListAllocator&& rhs) noexcept;

	void* Allocate(const std::size_t& size, const std::uintptr_t& alignment) override;

	void Free(void* ptr) noexcept;
}


struct FreeBlock {
	std::size_t size;
	FreeBlock* next;	
};

struct AllocationHeader {
	std::size_t size;
	std::uintptr_t adjustment;
};
