//
// Created by shahar on 15/01/17.
//


#include <packets/Packet.h>

string WRQ::getFileName() {
    return this->fileName;
}

int WRQ::getSize() {
    return 2+fileName.length()+1;
}