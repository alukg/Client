#ifndef CLIENT_NONBLOCKINGQUEUE_H
#define CLIENT_NONBLOCKINGQUEUE_H

#include <mutex>
#include <queue>
#include <condition_variable>
#include <packets/Packet.h>

class NonBlockingQueue {

private:
    std::queue <Packet> queue_;
    std::mutex mutex_;

public:
    NonBlockingQueue();
    Packet pop();
    void push(const Packet& item);
    bool isEmpty();
};

#endif //CLIENT_NONBLOCKINGQUEUE_H
