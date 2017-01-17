//
// Created by shahar on 15/01/17.
//

#include <packets/Packet.h>

short ACK::getBlock() {
    return this->block;
}

int ACK::getSize() {
    return 4;
}

