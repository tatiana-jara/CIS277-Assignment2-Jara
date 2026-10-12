#include "PacketQueue.h"
#include "Queue.h"
#include <cassert>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

void showCounts(const PacketQueue& queue) 
{
    std::cout << "Pending=" << queue.pending()
              << ", Available blocks=" << queue.availableBlocks() << '\n';
}

void showProcessed(PacketQueue& queue, std::size_t expectedId) 
{
    std::size_t id = 0;
    std::vector<unsigned char> bytes;
    assert(queue.processNext(id, bytes) && id == expectedId);
    std::cout << "Processed ID=" << id << " bytes:";
    for (unsigned char byte : bytes) {
        std::cout << " 0x" << std::hex << std::uppercase
                  << std::setw(2) << std::setfill('0')
                  << static_cast<unsigned int>(byte) << std::dec;
    }
    std::cout << '\n';
    showCounts(queue);
}

void testQueueADT() 
{
    Queue<int> numbers(2);
    int out = -1;
    assert(!numbers.dequeue(out) && out == -1);
    assert(numbers.enqueue(10) && numbers.enqueue(20));
    assert(!numbers.enqueue(30));
    assert(numbers.front(out) && out == 10 && numbers.size() == 2);
    assert(numbers.dequeue(out) && out == 10);
    assert(numbers.enqueue(30)); // Wraparound.
    assert(numbers.dequeue(out) && out == 20);
    assert(numbers.dequeue(out) && out == 30);
    assert(numbers.empty());

    Queue<std::string> words(2);
    std::string text;
    assert(words.enqueue("alpha") && words.enqueue("beta"));
    assert(words.dequeue(text) && text == "alpha");
    assert(words.enqueue("gamma"));
    assert(words.dequeue(text) && text == "beta");
    assert(words.dequeue(text) && text == "gamma");
    std::cout << "PASS: own tests for int/string Queue, FIFO, front, overflow and wraparound\n";
}

int main() 
{
    testQueueADT();
    PacketQueue queue(4, 3);
    const unsigned char packet1[] = {0x00, 0xFF, 0x12, 0x34};
    const unsigned char packet2[] = {0x55};
    const unsigned char packet3[] = {0x00, 0x80};
    const unsigned char packet4[] = {0xAA, 0xBB, 0xCC};
    showCounts(queue);

    assert(!queue.receive(99, nullptr, 1));
    assert(!queue.receive(99, packet1, 0));
    assert(!queue.receive(99, packet1, 5));
    assert(queue.pending() == 0 && queue.availableBlocks() == 3);
    std::cout << "PASS: invalid packets rejected without changing state\n";

    assert(queue.receive(1, packet1, 4));
    assert(queue.receive(2, packet2, 1));
    assert(queue.receive(3, packet3, 2));
    showCounts(queue);
    assert(!queue.receive(4, packet4, 3));
    assert(queue.pending() == 3 && queue.availableBlocks() == 0);
    std::cout << "PASS: full queue rejects packet 4\n";

    showProcessed(queue, 1);
    assert(queue.receive(4, packet4, 3));
    std::cout << "Packet 4 arrived after processing packet 1.\n";
    std::cout << "The pool may reuse a LIFO block, but packet order stays FIFO.\n";
    showCounts(queue);
    showProcessed(queue, 2);
    showProcessed(queue, 3);
    showProcessed(queue, 4);

    std::size_t id = 888;
    std::vector<unsigned char> bytes{0x77};
    assert(!queue.processNext(id, bytes));
    assert(id == 888 && bytes == std::vector<unsigned char>{0x77});
    assert(queue.pending() == 0 && queue.availableBlocks() == 3);
    std::cout << "PASS: empty process leaves outputs unchanged\n";
    std::cout << "PASS: all demonstration tests\n";
}
