# CIS-277 Assignment 2: Network Packet Arrival Queue

## Student
Tatiana Jara — CIS-277

## Design

This project implements a custom Queue ADT using a fixed-capacity circular array to process network packets in First-In, First-Out (FIFO) order.

**1. Queue ADT**

The `Queue<T>` class uses a `std::vector<T>` as fixed-size storage. It keeps track of two variables:

- `frontIndex_`: The position of the first element in the queue.
- `count_`: The number of elements currently stored.

The `enqueue()` method adds an element at the rear of the queue, while `dequeue()` removes the oldest element. The `front()` method returns the first element without removing it.

The circular array uses the modulo operator (`%`) to wrap around to the beginning when the end of the array is reached. This avoids shifting elements and allows queue operations to run in O(1) time, excluding the cost of copying an element.

**2. MemoryPool**

I reused my `Stack.h`, `MemoryPool.h`, and `MemoryPool.cpp` files from Assignment 1.

The `MemoryPool` class manages memory blocks using a Stack with Last-In, First-Out (LIFO) behavior. The `allocate()` method provides an available block, and `deallocate()` returns a block to the pool.

**3. PacketQueue**

The `PacketQueue` class combines the Queue ADT and MemoryPool.

Each packet descriptor contains a packet ID, a pointer to its memory block, and the number of bytes stored.

When a packet arrives, `receive()` checks whether the packet is valid, allocates a memory block, copies the bytes, and adds the descriptor to the queue.

When processing a packet, `processNext()` retrieves the oldest packet, copies its bytes into the output vector, removes it from the queue, and releases its memory block.

The Queue controls the processing order (FIFO), while the MemoryPool manages memory reuse (LIFO). This ensures that packets are processed in arrival order even when memory blocks are reused.

## How to Compile
Requires a C++23-capable compiler.

```bash
g++ -std=c++23 -Wall -Wextra -pedantic main.cpp PacketQueue.cpp MemoryPool.cpp -o packet_queue
```

## How to Run

After compiling the program, execute it using the following command:

**Windows:**
```bash
.\packet_queue.exe
```

**Linux/macOS:**
```bash
./packet_queue
```

To run the instructor's validator:

```bash
python validate.py
```

If Python is installed as `python3`, use:

```bash
python3 validate.py
```

The validator checks the project files, compilation, and queue functionality.

GitHub Actions uses GCC 14 for C++23.

## Tests and Results

I tested the Queue ADT and PacketQueue implementation using the program in `main.cpp` and the instructor's public validator.

| Test Case | Expected Result | Actual Result |
|---|---|---|
| Queue with integers and strings | Elements are processed in FIFO order | PASS |
| Circular wraparound | Queue reuses available positions correctly | PASS |
| Front operation | Returns the first element without removing it | PASS |
| Queue overflow and underflow | Invalid operations are rejected without changing the state | PASS |
| Invalid and oversized packets | Packets are rejected without changing the queue | PASS |
| Binary data | Values `0x00` and `0xFF` are preserved | PASS |
| Packet processing | Packets are processed in arrival order | PASS |
| Memory block reuse | Released memory is reused while maintaining FIFO order | PASS |

The public preflight validator passed using GCC 14 with C++23 during initial local validation. The final GitHub Actions result should be checked after committing the project.

## Analysis Questions
**1. Why FIFO for packets and LIFO for blocks?** 
Packet handling must respect arrival order. Memory-block reuse is independent and can reuse the most recently freed block first.

**2. How does wraparound work?** 
The next insertion index is `(frontIndex + count) % capacity`, and dequeue moves the front index `(frontIndex + 1) % capacity`. 
This reuses space without shifting waiting elements.

**3. Overflow/underflow?**  
A full queue rejects enqueue and an empty queue rejects dequeue/front. 
The queue and output arguments remain unchanged on failure, avoiding data loss or incorrect state.

**4. Complexity?** 
Each `Queue` operation is O(1), excluding the cost of copying the stored object. 
A successful packet `receive` and `processNext` are O(length) because the packet bytes must be copied. Space for packet storage is O(blockSize × blockCount).

**5. Why copy before release?** 
A returned block can be reused and overwritten immediately. 
The output must own an independent copy before the pool reclaims the block.

## AI Use
I used ChatGPT to help me understand how the data structures in this assignment work and to assist with the implementation and documentation.
For example, ChatGPT helped me understand how a circular queue uses the modulo operator to reuse positions without shifting elements, and why memory blocks must be released after copying packet data.
