#include <cstddef>
#include <cstdint>


class Allocator {

public:

	Allocator(const std::size_t sizeBytes, void* const start) noexcept;
	
	Allocator(const Allocator&) = delete;
	Allocator& operator=(Allocator&) = delete;
	
	Allocator(Allocator&&) noexcept;
	Allocator& operator=(Allocator&&) noexcept;

	virtual ~Allocator() noexcept;
	
	virtual void* Allocate(const std::size_t& size, 
		const std::uintptr_t& alignment = sizeof(std::intptr_t)) = 0;
	virtual void Free(void* const ptr) = 0;

	std::size_t& GetSize() const noexcept;
	std::size_t& GetUsed() const noexcept;
	std::size_t& GetNumAllocations() const noexcept;

	const void* GetStart() const noexcept;


protected:

	std::size_t m_size;
	std::size_t m_numAllocations;
	std::size_t m_usedBytes;

	void* m_start;

};
