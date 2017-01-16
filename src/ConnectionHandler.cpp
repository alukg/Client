#include <packets/Packet.h>
#include <fstream>

#include "ConnectionHandler.h"

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

ConnectionHandler::ConnectionHandler(string host, short port) : host_(host), port_(port), io_service_(),
                                                                socket_(io_service_), sendToServerQueue(), dataQueue() {}

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
    while (true) {
        if (lastPacketISent == nullptr) {
            char *function = sendToServerQueue.pop();
            if (function != nullptr) {
                if (!sendBytes(function, sizeof(function))) {
                    std::cout << "Disconnected. Exiting...\n" << std::endl;
                    break;
                }
            }
        }
        Packet *packet = getLine();
        if (packet != nullptr) {
            Packet response = process(packet);
            if (&response != nullptr) {
                char *encodedData = encode(response);
                sendBytes(encodedData, sizeof(encodedData));

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
    Packet *pck;
    char ch;
    string line;
    // Stop when we encounter the null character.
    // Notice that the null character is not appended to the frame string.
    try {
            getBytes(&ch, 2);
            line.append(2, ch);
        short opCode = bytesToShort((char *) line.c_str());
        line.clear();
        switch (opCode) {
            case 3:
                getBytes(&ch, 2);
                line.append(2, ch);
                short packetSize = bytesToShort((char *) line.substr(0,2).c_str());
                getBytes((char *)line.c_str(),packetSize+2);
                pck = new DATA(packetSize,bytesToShort((char *) line.substr(2,4).c_str()),(char *)line.substr(4,line.length()).c_str());
                return pck;
            case 4:
                getBytes(&ch, 2);
                line.append(2, ch);
                pck = new ACK(bytesToShort((char *) line.c_str()));
                return pck;
            case 5:
            case 9:
                do {
                    getBytes(&ch, 1);
                    line.append(1, ch);
                } while (ch != '0');
                if(opCode ==5) {
                    pck = new ERROR(bytesToShort((char *) line.substr(0, 2).c_str()),
                                            line.substr(2, line.length()));
                    return pck;
                } else{
                    pck = new BCAST(bytesToShort((char *) line.substr(0, 1).c_str()),
                                            line.substr(1, line.length()));
                    return pck;
                }
        }

    } catch (std::exception &e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
    }
    return nullptr;
}

char* ConnectionHandler::encode(Packet &packet){
    char result[2];
    shortToBytes(packet.getOpCode(),result);

    char blockArr[2];
    char packetSize[2];
    char fileNameWithZero[1024];
    char userNameWithZero[1024];
    char* ans;

    switch (packet.getOpCode()){
        case 1:
            RRQ RRQPack = (RRQ&)packet;
            strcpy(fileNameWithZero, (RRQPack.getFileName() + "0").c_str());
            return connectArrays(result,fileNameWithZero);
        case 2:
            WRQ WRQPack = (WRQ&)packet;
            strcpy(fileNameWithZero, (WRQPack.getFileName() + "0").c_str());
            return connectArrays(result,fileNameWithZero);
        case 3:
            DATA DATAPack = (DATA&)packet;
            shortToBytes(DATAPack.getBlock(),blockArr);
            shortToBytes(DATAPack.getPacketSize(),packetSize);
            ans = connectArrays(result,blockArr);
            ans = connectArrays(ans,packetSize);
            return connectArrays(ans,DATAPack.getData());
        case 4:
            ACK ACKPack = (ACK&)packet;
            shortToBytes(ACKPack.getBlock(),blockArr);
            return connectArrays(result,blockArr);
        case 7:
            LOGRQ LOGRQPack = (LOGRQ&)packet;
            strcpy(userNameWithZero, (LOGRQPack.getUserName() + "0").c_str());
            return connectArrays(result,userNameWithZero);
        case 8:
            DELRQ DELRQPack = (DELRQ&)packet;
            strcpy(fileNameWithZero, (DELRQPack.getFileName() + "0").c_str());
            return connectArrays(result,fileNameWithZero);
        case 6: //DIRQ
        case 10: //DISC
            return result;
    }

}

void ConnectionHandler::insertToQueue(char* message){
    sendToServerQueue.push(message);
}

Packet ConnectionHandler::process(Packet *packet) {
    switch (packet->getOpCode()) {
        case 3:
            ACK *ack = ((ACK *) packet);
            if(lastPacketISent->getOpCode() == 2) {
                WRQ *wrq = ((WRQ *) packet);
                if (ack->getBlock() != 0) {
                    cout << "ACK " << ack->getBlock() << endl;
                    if (dataQueue.empty()) {
                        cout << "WRQ " <<wrq->getFileName() << " complete" <<  endl;
                        lastPacketISent = nullptr;
                        return BCAST(1,wrq->getFileName());
                    } else{
                        return dataQueue.front();
                    }

                }
                else {
                    char* data = readFileBytes((char*)wrq->getFileName().c_str());
                    addDatatoQueue(data);
                    return dataQueue.front();
                }
                }
            if(lastPacketISent->getOpCode() == 10){
                close();
            }

        case 4:
            if(lastPacketISent->getOpCode() == 6){
                DATA* data = ((DATA*)packet);

            }
            if(lastPacketISent->getOpCode() == 1){

            }

        case 5:
            cout << "Error " << ((ERROR*)packet)->getErrorCode() << " " <<((ERROR*)packet)->getErrorMessage() << endl;

        case 9:
            string deloradd;
            if(((BCAST*)packet)->isDelOrAdd())
                deloradd = "add";
            else
                deloradd = "del";
            cout << "BCAST" << deloradd << ((BCAST*)packet)->getFileName() << endl;

    }
}


// Close down the connection properly.
void ConnectionHandler::close() {
    try {
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}
char* ConnectionHandler::readFileBytes(const char *name)
{
    ifstream fl(name);
    fl.seekg( 0, ios::end );
    size_t len = fl.tellg();
    char *ret = new char[len];
    fl.seekg(0, ios::beg);
    fl.read(ret, len);
    fl.close();
    return ret;
}

void ConnectionHandler::addDatatoQueue(char *data) {
    int len = sizeof(data), block = 0;
    while(len>512){
        char buff[512];
        memcpy(buff,data+(512*(block)),512);
        dataQueue.push(DATA(512,block+1,buff));
        len -=512;
        block++;
    }
    if(len>0){
        char abuff[len];
        memcpy(abuff,data + (sizeof(data)- len),len);
        dataQueue.push(DATA(len,block,abuff));
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

char* ConnectionHandler::connectArrays(char *firstArr, char *secondArr) {

}