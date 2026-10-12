#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <cstddef>
#include "Stack.h"

class MemoryPool
{
private:
    std::size_t blockSize_;
    std::size_t blockCount_;
    std::size_t allocatedCount_;
    unsigned char* memory_;
    bool* inUse_;
    Stack<void*> freeBlocks_;
public:
    MemoryPool(std::size_t blockSize, std::size_t blockCount);
    ~MemoryPool();
    void* allocate();
    bool deallocate(void* ptr);
    std::size_t availableBlocks() const;
    std::size_t allocatedBlocks() const;
    std::size_t blockSize() const;
    std::size_t capacity() const;
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;
};
#endif
