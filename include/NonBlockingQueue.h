#ifndef CLIENT_NONBLOCKINGQUEUE_H
#define CLIENT_NONBLOCKINGQUEUE_H

#include <mutex>
#include <queue>
#include <condition_variable>

template <typename T>
class NonBlockingQueue {

private:
    std::queue <T> queue_;
    std::mutex mutex_;

public:
    NonBlockingQueue();
    T pop();
    void push(const T& item);
    bool isEmpty();
};

#endif //CLIENT_NONBLOCKINGQUEUE_H
