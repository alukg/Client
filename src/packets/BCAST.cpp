//
// Created by shahar on 15/01/17.
//

#include <packets/Packet.h>

bool BCAST::isDelOrAdd() {
    return this->delOrAdd;
}
string BCAST::getFileName() {
    return this->fileName;
}