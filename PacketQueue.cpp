#include "PacketQueue.h"
#include <cstring>

PacketQueue::PacketQueue(std::size_t blockSize, std::size_t blockCount)
    : blockSize_(blockSize), pool_(blockSize, blockCount), queue_(blockCount) {}

bool PacketQueue::receive(std::size_t id, const unsigned char* bytes,
                          std::size_t length) {
    if (bytes == nullptr || length == 0 || length > blockSize_ ||
        queue_.full() || pool_.availableBlocks() == 0) {
        return false;
    }

    unsigned char* block = static_cast<unsigned char*>(pool_.allocate());
    if (block == nullptr) return false;

    std::memcpy(block, bytes, length);
    PacketDescriptor packet{id, block, length};
    if (!queue_.enqueue(packet)) {
        pool_.deallocate(block);
        return false;
    }
    return true;
}

bool PacketQueue::processNext(std::size_t& id,
                              std::vector<unsigned char>& bytes) {
    PacketDescriptor packet;
    if (!queue_.front(packet)) return false;

    // Copy before releasing so memory reuse cannot overwrite packet data.
    std::vector<unsigned char> copied(packet.block, packet.block + packet.length);
    if (!queue_.dequeue(packet)) return false;
    id = packet.id;
    bytes.swap(copied);
    pool_.deallocate(packet.block);
    return true;
}

std::size_t PacketQueue::pending() const { return queue_.size(); }
std::size_t PacketQueue::availableBlocks() const { return pool_.availableBlocks(); }
