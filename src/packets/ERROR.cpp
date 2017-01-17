//
// Created by shahar on 15/01/17.
//


#include <packets/Packet.h>

short ERROR::getErrorCode() {
    return this->errorCode;
}
string ERROR::getErrorMessage() {
    return this->errorMessage;
}

int ERROR::getSize() {
    return 2+2+errorMessage.length()+1;
}