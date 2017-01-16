//
// Created by shahar on 15/01/17.
//

#include <packets/Packet.h>

short DATA::getBlock() {
    return this->block;
}
short DATA::getPacketSize() {
    return this->packetSize;
}
char* DATA::getData() {
    char ans[sizeof(data)];
    std::copy(data, data+sizeof(data), ans);
    return ans;
}