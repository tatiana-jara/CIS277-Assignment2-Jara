#ifndef PACKET_QUEUE_H
#define PACKET_QUEUE_H

#include "MemoryPool.h"
#include "Queue.h"
#include <cstddef>
#include <vector>

struct PacketDescriptor {
    std::size_t id = 0;
    unsigned char* block = nullptr;
    std::size_t length = 0;
};

class PacketQueue {
private:
    std::size_t blockSize_;
    MemoryPool pool_;
    Queue<PacketDescriptor> queue_;
public:
    PacketQueue(std::size_t blockSize, std::size_t blockCount);
    bool receive(std::size_t id, const unsigned char* bytes, std::size_t length);
    bool processNext(std::size_t& id, std::vector<unsigned char>& bytes);
    std::size_t pending() const;
    std::size_t availableBlocks() const;
};
#endif
