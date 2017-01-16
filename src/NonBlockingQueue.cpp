#include "NonBlockingQueue.h"

NonBlockingQueue::NonBlockingQueue() : queue_(),mutex_() {}

Packet NonBlockingQueue::pop() {
    std::unique_lock<std::mutex> mlock(mutex_);
    Packet item = queue_.front();
    queue_.pop();
    return item;
}

void NonBlockingQueue::push(const Packet &item) {
    std::unique_lock<std::mutex> mlock(mutex_);
    queue_.push(item);
}
bool NonBlockingQueue::isEmpty() {
    return queue_.size()==0;
}

