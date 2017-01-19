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
                                                                socket_(io_service_), dataForSendQueue(),
                                                                lastPacketISent(nullptr),
                                                                gettingData(nullptr), gettingDataSize(),
                                                                shouldTerminate(false){}

ConnectionHandler::~ConnectionHandler() {
    delete[] gettingData;
    delete lastPacketISent;
    close();
}

Packet *ConnectionHandler::getLastPacketISent() {
    return lastPacketISent;
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
                if(response->getOpCode() == 3)
                    delete[] ((DATA*)response)->getData();
                delete response;
            }
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
            tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, (size_t) bytesToWrite - tmp), error);
        }
        if (error)
            throw boost::system::system_error(error);
    } catch (std::exception &e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

Packet* ConnectionHandler::getLine() {
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
        if (opCode == 3) {
            for (int a = 1; a <= 2; a++) {
                getBytes(&ch, 1);
                line.append(1, ch);
            }
            packetSize = bytesToShort((char *) line.substr(0, 2).c_str());
            for (int a = 1; a <= packetSize + 2; a++) {
                getBytes(&ch, 1);
                line.append(1, ch);
            }
            char *result = new char[line.length() - 4];
            for (unsigned int i = 4; i < line.length(); i++) {
                result[i-4] = line[i];
            }
            return new DATA(packetSize, bytesToShort((char *) line.substr(2, 4).c_str()), result);
        } else if (opCode == 4) {
            for (int a = 1; a <= 2; a++) {
                getBytes(&ch, 1);
                line.append(1, ch);
            }

            return new ACK(bytesToShort((char *) line.c_str()));
        } else if (opCode == 5) {
            getBytes(&ch, 1);
            line.append(1, ch);
            getBytes(&ch, 1);
            line.append(1, ch);
            do {
                getBytes(&ch, 1);
                line.append(1, ch);
            } while (ch != '\0');
            return new ERROR(bytesToShort((char *) line.substr(0, 2).c_str()), line.substr(2, line.length()));
        } else if (opCode == 9) {
            getBytes(&ch, 1);
            line.append(1, ch);
            do {
                getBytes(&ch, 1);
                line.append(1, ch);
            } while (ch != '\0');

            if(line.substr(0, 1) == "1")
                return new BCAST(true, line.substr(1, line.length()));
            else
                return new BCAST(false, line.substr(1, line.length()));
        }
    } catch (std::exception &e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
    }
    return nullptr;
}

void ConnectionHandler::encode(Packet *packet, char *encodedArr) {
    char result[2];
    shortToBytes(packet->getOpCode(), result);

    char zero[1];
    zero[0] = '\0';
    char blockArr[2];
    char packetSize[2];
    char fileNameWithZero[512];
    char userNameWithZero[512];

    short opCode = packet->getOpCode();

    if (opCode == 1) {
        RRQ *RRQPack = (RRQ *) packet;
        strcpy(fileNameWithZero, (RRQPack->getFileName() + '\0').c_str());
        connectArrays(result, 2, fileNameWithZero, 512, encodedArr);
    } else if (opCode == 2) {
        WRQ *WRQPack = (WRQ *) packet;
        strcpy(fileNameWithZero, (WRQPack->getFileName() + '\0').c_str());
        connectArrays(result, 2, fileNameWithZero, 512, encodedArr);
    } else if (opCode == 3) {
        DATA *DATAPack = (DATA *) packet;
        shortToBytes(DATAPack->getBlock(), blockArr);
        shortToBytes(DATAPack->getPacketSize(), packetSize);
        char temp[4];
        connectArrays(result, 2, packetSize, 2, temp);
        char temp2[6];
        connectArrays(temp, 4, blockArr, 2, temp2);
        connectArrays(temp2, 6, DATAPack->getData(), DATAPack->getPacketSize(), encodedArr);
    } else if (opCode == 4) {
        ACK *ACKPack = (ACK *) packet;
        shortToBytes(ACKPack->getBlock(), blockArr);
        connectArrays(result, 2, blockArr, 2, encodedArr);
    } else if (opCode == 7) {
        LOGRQ *LOGRQPack = (LOGRQ *) packet;
        connectArrays(LOGRQPack->getUserName().c_str(), LOGRQPack->getUserName().length(), zero, 1, userNameWithZero);
        connectArrays(result, 2, userNameWithZero, 512, encodedArr);
    } else if (opCode == 8) {
        DELRQ *DELRQPack = (DELRQ *) packet;
        strcpy(fileNameWithZero, (DELRQPack->getFileName() + '\0').c_str());
        connectArrays(result, 2, fileNameWithZero, 512, encodedArr);
    } else { //DIRQ and DISC
        memcpy(encodedArr, result, 2);
    }
}

Packet *ConnectionHandler::process(Packet &packet) {
    if (packet.getOpCode() == 4) { //ACK
        ACK& ack = dynamic_cast<ACK&>(packet);
        cout << "ACK " << ack.getBlock() << endl; // Print ACK n
        if (lastPacketISent->getOpCode() == 2) { // WRQ
            WRQ *wrq = (WRQ *) lastPacketISent;
            if (ack.getBlock() != 0) { // ACK != 0, continue send
                if (dataForSendQueue.empty()) {
                    cout << "WRQ " << wrq->getFileName() << " complete" << endl;
                    delete lastPacketISent;
                    lastPacketISent = nullptr;
                } else {
                    DATA *dataPacket = dataForSendQueue.front();
                    dataForSendQueue.pop();
                    return dataPacket;
                }
            } else { // ACK 0, read the file and start send
                int size;
                char *result = readFileBytes((char *) wrq->getFileName().c_str(), size);
                convertDataToPackets(result, size);
                delete result;
                DATA *dataPacket = dataForSendQueue.front();
                dataForSendQueue.pop();
                return dataPacket;
            }
        } else if (lastPacketISent->getOpCode() == 10) { // DISC
            delete lastPacketISent;
            lastPacketISent = nullptr;
            shouldTerminate = true;
        }
    } else if (packet.getOpCode() == 3) { // DATA
        DATA& data = dynamic_cast<DATA&>(packet);
        if (data.getBlock() == 1) { // First block
            delete[] gettingData;
            gettingData = nullptr;
            gettingData = new char[data.getPacketSize()];
            memcpy(gettingData, data.getData(), data.getPacketSize());
            delete[] data.getData();
            gettingDataSize = data.getPacketSize();
        } else {
            char *temp = new char[gettingDataSize + data.getPacketSize()];
            connectArrays(gettingData, gettingDataSize, data.getData(), data.getPacketSize(), temp);
            delete[] gettingData;
            gettingData = temp;
            delete[] data.getData();
            gettingDataSize = gettingDataSize + data.getPacketSize();
        }
        if (data.getPacketSize() < 512) { // if it's the last
            if (lastPacketISent->getOpCode() == 6) { // DIRQ
                for (int i = 0; i < gettingDataSize; i++) {
                        cout << *(gettingData + i);
                }
                cout.flush();
            }
            if (lastPacketISent->getOpCode() == 1) { // RRQ
                std::ofstream file(((RRQ *) lastPacketISent)->getFileName());
                if (file) {
                    char arr[gettingDataSize];
                    for (int i = 0; i < gettingDataSize; i++) {
                        arr[i] = *(gettingData + i);
                    }
                    file.write(&arr[0], gettingDataSize);
                }
                file.close();
                cout << "RRQ " << ((RRQ *) lastPacketISent)->getFileName() << " complete" << endl;
            }
            delete[] gettingData;
            gettingData = nullptr;
            delete lastPacketISent;
            lastPacketISent = nullptr;
        }
        return new ACK(data.getBlock());
    } else if (packet.getOpCode() == 5) { // ERROR
        cout << "Error " << ((ERROR &) packet).getErrorCode() << " " << ((ERROR &) packet).getErrorMessage()
             << endl;
    } else { // BCAST
        string delOrAdd;
        if (((BCAST &) packet).isDelOrAdd())
            delOrAdd = "add";
        else
            delOrAdd = "del";
        cout << "BCAST " << delOrAdd << " " << ((BCAST &) packet).getFileName() << endl;
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

char *ConnectionHandler::readFileBytes(char const *filename, int &arrSize) {
    ifstream fstream(filename);
    if (fstream.is_open()) {
        std::filebuf *pbuf = fstream.rdbuf();
        std::size_t size = pbuf->pubseekoff(0, fstream.end, fstream.in); //get length of the file
        arrSize = size;
        pbuf->pubseekpos(0, fstream.in);

        char *memblock = new char[size];
        pbuf->sgetn(memblock, size);
        fstream.close();

        return memblock;
    } else {
        cout << "Unable to open file" << endl;
        return nullptr;
    }
}

void ConnectionHandler::convertDataToPackets(char *data, int len) {
    short block = 0;
    int startLen = len;
    while (len > 512) {
        char* buff = new char[512];
        for (int i = 512 * (block); i < 512 * (block + 1); i++) {
            *(buff + i - (512 * (block))) = *(data + i);
        }
        dataForSendQueue.push(new DATA(512, block + 1, buff));
        len -= 512;
        block++;
    }
    if (len > 0) {
        char* abuff = new char[512];
        for (int i = startLen - len; i < startLen; i++) {
            *(abuff + i - (startLen - len)) = *(data + i);
        }
        dataForSendQueue.push(new DATA((short) len, block, abuff));
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
ConnectionHandler::connectArrays(const char *firstArr, int firstLength, const char *secondArr, int secondLength,
                                 char *connected) {
    memcpy(connected, firstArr, firstLength);
    memcpy(connected + firstLength, secondArr, secondLength);
}