//
// Created by shahar on 15/01/17.
//


#include <packets/Packet.h>

string RRQ::getFileName() {
    return this->fileName;
}

int RRQ::getSize() {
    return 2+fileName.length()+1;
}