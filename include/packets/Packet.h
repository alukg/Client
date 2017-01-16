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

};
class DATA : public Packet{
private:
    short packetSize;
    short block;
    char data[];
public:
    DATA(short packetSize, short block, char data[]) :Packet(3), packetSize(packetSize),block(block),data(data){};
    short getPacketSize();
    short getBlock();
    char* getData();
};

class ACK : public Packet {
private:
    short block;

public:
    ACK(short block): Packet((short)4),block(block){};
    short getBlock();
};


class BCAST : public Packet {
private:
    bool delOrAdd;
    string fileName;

public:
    BCAST(bool delOrAdd, string fileName): Packet(9), delOrAdd(delOrAdd), fileName(fileName){};
    bool isDelOrAdd();
    string getFileName();
};

class DELRQ : public Packet {
private:
    string FileName;

public:
    DELRQ(string FileName): Packet(8), FileName(FileName) {};
    string getFileName();
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
};

class LOGRQ : public Packet {
private:
    string userName;

public:
    LOGRQ(string userName): Packet(7),userName(userName) {};
    string getUserName();
};

class RRQ : public Packet {
private:
    string FileName;

public:
    RRQ(string FileName): Packet(1), FileName(FileName) {};
    string getFileName();
};

class WRQ : public Packet {
private:
    string FileName;

public:
    WRQ(string FileName): Packet(2),FileName(FileName) {};
    string getFileName();
};
#endif //CLIENT_PACKET_H
