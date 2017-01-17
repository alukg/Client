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
    char ans[packetSize];
    std::copy(data, data+packetSize, ans);
    return ans;
}
int DATA::getSize() {
    return 2+2+2+packetSize;
}