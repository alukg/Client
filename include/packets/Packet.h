//
// Created by shahar on 15/01/17.
//

#ifndef CLIENT_PACKET_H
#define CLIENT_PACKET_H

#include <string>

using namespace std;

class Packet {
private:
    short opCode;
public:
    Packet(short opCode) :opCode(opCode){};
    short getOpCode();
    virtual int getSize();
};

class DATA : public Packet{
private:
    short packetSize;
    short block;
    string data;
public:
    DATA(short packetSize, short block, string data) :Packet(3), packetSize(packetSize),block(block),data(data){};
    short getPacketSize();
    short getBlock();
    string getData();
    int getSize();
};

class ACK : public Packet {
private:
    short block;
public:
    ACK(short block): Packet((short)4),block(block){};
    short getBlock();
    int getSize();
};


class BCAST : public Packet {
private:
    bool delOrAdd;
    string fileName;
public:
    BCAST(bool delOrAdd, string fileName): Packet(9), delOrAdd(delOrAdd), fileName(fileName){};
    bool isDelOrAdd();
    string getFileName();
    int getSize();
};

class DELRQ : public Packet {
private:
    string fileName;
public:
    DELRQ(string FileName): Packet(8), fileName(FileName) {};
    string getFileName();
    int getSize();
};

class DIRQ : public Packet {
public:
    DIRQ(): Packet(6) {};
};

class DISC : public Packet {
public:
    DISC(): Packet(10) {};
};


class ERROR : public Packet {
private:
    short errorCode;
    string errorMessage;
public:
    ERROR(short errorCode, string errorMessage): Packet(5),errorCode(errorCode),errorMessage(errorMessage) {};
    short getErrorCode();
    string getErrorMessage();
    int getSize();
};

class LOGRQ : public Packet {
private:
    string userName;
public:
    LOGRQ(string userName): Packet(7),userName(userName) {};
    string getUserName();
    int getSize();
};

class RRQ : public Packet {
private:
    string fileName;
public:
    RRQ(string FileName): Packet(1), fileName(FileName) {};
    string getFileName();
    int getSize();
};

class WRQ : public Packet {
private:
    string fileName;
public:
    WRQ(string FileName): Packet(2),fileName(FileName) {};
    string getFileName();
    int getSize();
};
#endif //CLIENT_PACKET_H
