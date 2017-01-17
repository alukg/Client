//
// Created by shahar on 15/01/17.
//


#include <packets/Packet.h>

string DELRQ::getFileName() {
    return this->fileName;
}

int DELRQ::getSize() {
    return 2+fileName.length()+1;
}