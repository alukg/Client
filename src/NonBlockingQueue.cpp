#include "NonBlockingQueue.h"

NonBlockingQueue::NonBlockingQueue() : queue_(),mutex_() {}

template <typename T>
T NonBlockingQueue::pop() {
    std::unique_lock<std::mutex> mlock(mutex_);
    T item = queue_.front();
    queue_.pop();
    return item;
}

template <typename T>
void NonBlockingQueue::push(const T &item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
}
