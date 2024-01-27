#include "LinearAllocator.hpp"
#include <iostream>

int main() {
    const std::size_t poolSize = 1024; // Size of the memory pool in bytes
    char memoryPool[poolSize];         // Static memory pool for the allocator

    // Create an instance of LinearAllocator with the memory pool
    LinearAllocator allocator(poolSize, memoryPool);

    // Allocate some memory
    void* ptr1 = allocator.Allocate(100); // Allocate 100 bytes
    std::cout << "Allocated 100 bytes at " << ptr1 << std::endl;

    // Get the current position
    void* currentPos = allocator.GetCurrent();
    std::cout << "Current position: " << currentPos << std::endl;

    // Allocate more memory
    void* ptr2 = allocator.Allocate(50);  // Allocate 50 bytes
    std::cout << "Allocated 50 bytes at " << ptr2 << std::endl;

    // Rewind to the previous position
    allocator.Rewind(currentPos);
    std::cout << "Rewound to previous position" << std::endl;

    // Clear the allocator
    allocator.Clear();
    std::cout << "Allocator cleared" << std::endl;

    return 0;
}

