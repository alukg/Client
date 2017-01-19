//
// Created by shahar on 15/01/17.
//

#include <packets/Packet.h>

DATA::DATA(short packetSize, short block, char* data) :Packet(3), packetSize(packetSize),block(block),data(data){};

short DATA::getBlock() {
    return this->block;
}

short DATA::getPacketSize() {
    return this->packetSize;
}

char *DATA::getData() {
    return data;
}

int DATA::getSize() {
    return 2 + 2 + 2 + packetSize;
}