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
string DATA::getData() {
    return data;
}
int DATA::getSize() {
    return 2+2+2+packetSize;
}