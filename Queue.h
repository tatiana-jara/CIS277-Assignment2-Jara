#ifndef QUEUE_H
#define QUEUE_H

#include <cstddef>
#include <vector>
#include <stdexcept>

template <typename T>
class Queue {
private:
    std::vector<T> items_;
    std::size_t frontIndex_ = 0;
    std::size_t count_ = 0;

public:
    explicit Queue(std::size_t capacity) : items_(capacity) {
        if (capacity == 0) {
            throw std::invalid_argument("Queue capacity must be positive");
        }
    }

    bool enqueue(const T& value) {
        if (full()) return false;
        std::size_t rearIndex = (frontIndex_ + count_) % capacity();
        items_[rearIndex] = value;
        ++count_;
        return true;
    }

    bool dequeue(T& value) {
        if (empty()) return false;
        value = items_[frontIndex_];
        frontIndex_ = (frontIndex_ + 1) % capacity();
        --count_;
        return true;
    }

    bool front(T& value) const {
        if (empty()) return false;
        value = items_[frontIndex_];
        return true;
    }

    bool empty() const { return count_ == 0; }
    bool full() const { return count_ == capacity(); }
    std::size_t size() const { return count_; }
    std::size_t capacity() const { return items_.size(); }
};
#endif
