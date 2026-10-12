#include "MemoryPool.h"

#include <cstdint>
#include <limits>
#include <stdexcept>

MemoryPool::MemoryPool(std::size_t blockSize, std::size_t blockCount)
    : blockSize_(blockSize),
      blockCount_(blockCount),
      allocatedCount_(0),
      memory_(nullptr),
      inUse_(nullptr)
{
    if (blockSize_ == 0 || blockCount_ == 0)
    {
        throw std::invalid_argument(
            "Block size and block count must be greater than zero."
        );
    }
    if (blockCount_ >
        std::numeric_limits<std::size_t>::max() / blockSize_)
    {
        throw std::overflow_error(
            "Memory pool capacity is too large."
        );
    }
    memory_ = new unsigned char[capacity()];
    inUse_ = new bool[blockCount_]();
    for (std::size_t i = 0; i < blockCount_; ++i)
    {
        freeBlocks_.push(memory_ + i * blockSize_);
    }
}
MemoryPool::~MemoryPool()
{
    delete[] inUse_;
    delete[] memory_;
}
void* MemoryPool::allocate()
{
    if (freeBlocks_.empty())
    {
        return nullptr;
    }
    void* block = freeBlocks_.pop();
    unsigned char* bytePtr =
        static_cast<unsigned char*>(block);
    std::size_t index =
        static_cast<std::size_t>(
            bytePtr - memory_
        ) / blockSize_;
    inUse_[index] = true;
    ++allocatedCount_;
    return block;
}
bool MemoryPool::deallocate(void* ptr)
{
    if (ptr == nullptr)
    {
        return false;
    }
    std::uintptr_t start =
        reinterpret_cast<std::uintptr_t>(memory_);
    std::uintptr_t address =
        reinterpret_cast<std::uintptr_t>(ptr);
    std::uintptr_t totalBytes =
        static_cast<std::uintptr_t>(capacity());
    if (address < start ||
        address >= start + totalBytes)
    {
        return false;
    }
    std::uintptr_t offset =
        address - start;
    if (offset % blockSize_ != 0)
    {
        return false;
    }
    std::size_t index =
        static_cast<std::size_t>(
            offset / blockSize_
        );
    if (index >= blockCount_ ||
        !inUse_[index])
    {
        return false;
    }
    inUse_[index] = false;
    --allocatedCount_;
    freeBlocks_.push(ptr);
    return true;
}
std::size_t MemoryPool::availableBlocks() const
{
    return freeBlocks_.size();
}
std::size_t MemoryPool::allocatedBlocks() const
{
    return allocatedCount_;
}
std::size_t MemoryPool::blockSize() const
{
    return blockSize_;
}
std::size_t MemoryPool::capacity() const
{
    return blockSize_ * blockCount_;
}
