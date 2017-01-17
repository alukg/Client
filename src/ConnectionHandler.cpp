#include <packets/Packet.h>
#include <fstream>
#include <boost/algorithm/string/replace.hpp>

#include "ConnectionHandler.h"

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

ConnectionHandler::ConnectionHandler(string host, short port) : host_(host), port_(port), io_service_(),
                                                                socket_(io_service_), dataForSendQueue(), shouldTerminate(false),
                                                                lastPacketISent(nullptr) {}

ConnectionHandler::~ConnectionHandler() {
    close();
}

bool ConnectionHandler::connect() {
    std::cout << "Starting connect to "
              << host_ << ":" << port_ << std::endl;
    try {
        tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
        boost::system::error_code error;
        socket_.connect(endpoint, error);
        if (error)
            throw boost::system::system_error(error);
    }
    catch (std::exception &e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

void ConnectionHandler::run() {
    while (!shouldTerminate) {
        Packet *packet = getLine();
        if (packet != nullptr) {
            Packet *response = process(*packet);
            delete packet;
            if (response != nullptr) {
                sendPacket(response, false);
            }
            delete response;
        }
    }
}

bool ConnectionHandler::getBytes(char bytes[], unsigned int bytesToRead) {
    size_t tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToRead > tmp) {
            tmp += socket_.read_some(boost::asio::buffer(bytes + tmp, bytesToRead - tmp), error);
        }
        if (error)
            throw boost::system::system_error(error);
    } catch (std::exception &e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::sendPacket(Packet *packet, bool save) {
    if (save)
        lastPacketISent = packet;
    char encodedArr[1024];
    encode(packet, encodedArr);
    return sendBytes(encodedArr, packet->getSize());
}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp) {
            tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
        if (error)
            throw boost::system::system_error(error);
    } catch (std::exception &e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

Packet *ConnectionHandler::getLine() {
    char ch;
    string line;
    try {
        for (int a = 1; a <= 2; a++) {
            getBytes(&ch, 1);
            line.append(1, ch);
        }
        short opCode = bytesToShort((char *) line.c_str());
        line.clear();
        short packetSize;
        switch (opCode) {
            case 3:
                for (int a = 1; a <= 2; a++) {
                    getBytes(&ch, 1);
                    line.append(1, ch);
                }
                packetSize = bytesToShort((char *) line.substr(0, 2).c_str());
                for (int a = 1; a <= packetSize + 2; a++) {
                    getBytes(&ch, 1);
                    line.append(1, ch);
                }
                return new DATA(packetSize, bytesToShort((char *) line.substr(2, 4).c_str()),
                                line.substr(4, line.length()));
            case 4:
                for (int a = 1; a <= 2; a++) {
                    getBytes(&ch, 1);
                    line.append(1, ch);
                }
                return new ACK(bytesToShort((char *) line.c_str()));
            case 5:
            case 9:
                do {
                    getBytes(&ch, 1);
                    if (ch != '0')
                        line.append(1, ch);
                } while (ch != '0');
                if (opCode == 5) {
                    return new ERROR(bytesToShort((char *) line.substr(0, 2).c_str()),
                                     line.substr(2, line.length()));
                } else {
                    return new BCAST(bytesToShort((char *) line.substr(0, 1).c_str()),
                                     line.substr(1, line.length()));
                }
        }
    } catch (std::exception &e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
    }
    return nullptr;
}

void ConnectionHandler::encode(Packet *packet, char *encodedArr) {
    char *p;
    char result[2];
    p = result;
    shortToBytes(packet->getOpCode(), p);

    char blockArr[2];
    char packetSize[2];
    char fileNameWithZero[512];
    char userNameWithZero[512];

    short opCode = packet->getOpCode();

    if (opCode == 1) {
        RRQ *RRQPack = (RRQ *) packet;
        strcpy(fileNameWithZero, (RRQPack->getFileName() + "0").c_str());
        connectArrays(result, 2, fileNameWithZero, 512, encodedArr);
    } else if (opCode == 2) {
        WRQ *WRQPack = (WRQ *) packet;
        strcpy(fileNameWithZero, (WRQPack->getFileName() + "0").c_str());
        connectArrays(result, 2, fileNameWithZero, 512, encodedArr);
    } else if (opCode == 3) {
        DATA *DATAPack = (DATA *) packet;
        shortToBytes(DATAPack->getBlock(), blockArr);
        shortToBytes(DATAPack->getPacketSize(), packetSize);
        char temp[4];
        connectArrays(result, 2, blockArr, 2, temp);
        char temp2[6];
        connectArrays(temp, 4, packetSize, 2, temp2);
        connectArrays(temp2, 6, (char *) DATAPack->getData().c_str(), DATAPack->getPacketSize(), encodedArr);
    } else if (opCode == 4) {
        ACK *ACKPack = (ACK *) packet;
        shortToBytes(ACKPack->getBlock(), blockArr);
        connectArrays(result, 2, blockArr, 2, encodedArr);
    } else if (opCode == 7) {
        LOGRQ *LOGRQPack = (LOGRQ *) packet;
        strcpy(userNameWithZero, (LOGRQPack->getUserName() + "0").c_str());
        connectArrays(result, 2, userNameWithZero, 512, encodedArr);
    } else if (opCode == 8) {
        DELRQ *DELRQPack = (DELRQ *) packet;
        strcpy(fileNameWithZero, (DELRQPack->getFileName() + "0").c_str());
        connectArrays(result, 2, fileNameWithZero, 512, encodedArr);
    } else { //DIRQ and DISC
        memcpy(encodedArr, result, 2);
    }
}

Packet *ConnectionHandler::process(Packet &packet) {
    if (packet.getOpCode() == 4) { //ACK
        ACK ack = (ACK &) packet;
        cout << "ACK " << ack.getBlock() << endl; // Print ACK n
        if (lastPacketISent->getOpCode() == 2) { // WRQ
            WRQ wrq = (WRQ &) packet;
            if (ack.getBlock() != 0) { // ACK != 0, continue send
                if (dataForSendQueue.empty()) {
                    cout << "WRQ " << wrq.getFileName() << " complete" << endl;
                    delete lastPacketISent;
                    lastPacketISent = nullptr;
                } else {
                    DATA *dataPacket = dataForSendQueue.front();
                    dataForSendQueue.pop();
                    return dataPacket;
                }
            } else { // ACK 0, read the file and start send
                char *data = readFileBytes((char *) wrq.getFileName().c_str());
                convertDataToPackets(data);
                DATA *dataPacket = dataForSendQueue.front();
                dataForSendQueue.pop();
                return dataPacket;
            }
        } else if (lastPacketISent->getOpCode() == 10) { // DISC
            delete lastPacketISent;
            lastPacketISent = nullptr;
            close();
        }
    } else if (packet.getOpCode() == 3) { // DATA
        DATA data = (DATA &) packet;
        if (data.getBlock() == 1) { // First block
            gettingData = data.getData();
            gettingDataSize = data.getPacketSize();
        } else {
            gettingData = gettingData + data.getData();
        }
        if (data.getPacketSize() < 512) { // if it's the last
            char *result = (char *) gettingData.c_str();
            if (lastPacketISent->getOpCode() == 6) { // DIRQ
                for (int i = 0; i < gettingDataSize; i++) {
                    if (*(result + i) == '\0')
                        cout << endl;
                    else
                        cout << *(result + i);
                }
            }
            if (lastPacketISent->getOpCode() == 1) { // RRQ
                std::ofstream file(((RRQ &) (*lastPacketISent)).getFileName());
                if (file) {
                    file.write(&gettingData[0], gettingDataSize);
                }
                file.close();
            }
            delete lastPacketISent;
            lastPacketISent = nullptr;
        }
        return new ACK(data.getBlock());
    } else if (packet.getOpCode() == 5) { // ERROR
        cout << "Error " << ((ERROR &) packet).getErrorCode() << " " << ((ERROR &) packet).getErrorMessage()
             << endl;
        delete lastPacketISent;
        lastPacketISent = nullptr;
    } else { // BCAST
        string delOrAdd;
        if (((BCAST &) packet).isDelOrAdd())
            delOrAdd = "add";
        else
            delOrAdd = "del";
        cout << "BCAST" << delOrAdd << ((BCAST &) packet).getFileName() << endl;
        delete lastPacketISent;
        lastPacketISent = nullptr;
    }
    return nullptr;
}


// Close down the connection properly.
void ConnectionHandler::close() {
    try {
        socket_.close();

    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}

char *ConnectionHandler::readFileBytes(const char *name) {
    ifstream fl(name);
    fl.seekg(0, ios::end);
    size_t len = fl.tellg();
    char *ret = new char[len];
    fl.seekg(0, ios::beg);
    fl.read(ret, len);
    fl.close();
    return ret;
}

void ConnectionHandler::convertDataToPackets(char *data) {
    int len = sizeof(data), block = 0;
    while (len > 512) {
        char buff[512];
        memcpy(buff, data + (512 * (block)), 512);
        dataForSendQueue.push(new DATA(512, block + 1, buff));
        len -= 512;
        block++;
    }
    if (len > 0) {
        char abuff[len];
        memcpy(abuff, data + (sizeof(data) - len), len);
        dataForSendQueue.push(new DATA(len, block, abuff));
    }
}

short ConnectionHandler::bytesToShort(char *bytesArr) {
    short result = (short) ((bytesArr[0] & 0xff) << 8);
    result += (short) (bytesArr[1] & 0xff);
    return result;
}

void ConnectionHandler::shortToBytes(short num, char *bytesArr) {
    bytesArr[0] = ((num >> 8) & 0xFF);
    bytesArr[1] = (num & 0xFF);
}

void
ConnectionHandler::connectArrays(char *firstArr, int firstLength, char *secondArr, int secondLength, char *connected) {
    memcpy(connected, firstArr, firstLength);
    memcpy(connected + firstLength, secondArr, secondLength);
}