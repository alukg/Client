//
// Created by shahar on 15/01/17.
//


#include <packets/Packet.h>

string LOGRQ::getUserName() {
    return this->userName;
}

int LOGRQ::getSize() {
    return 2+userName.length()+1;
}