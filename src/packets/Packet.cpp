//
// Created by shahar on 15/01/17.
//

#include "packets/Packet.h"

Packet::~Packet(){}

short Packet::getOpCode() {
    return this->opCode;
}

int Packet::getSize(){
    return 2;
}