#include <packets/Packet.h>
#include "ConnectionHandler.h"

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

ConnectionHandler::ConnectionHandler(string host, short port) : host_(host), port_(port), io_service_(),
                                                                socket_(io_service_), sendToServerQueue() {}

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
    while(true){
        char* function = sendToServerQueue.pop();
        if(function != nullptr){
            if (!sendBytes(function)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
        }
        Packet& packet = getLine();
        if (packet == nullptr) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }

        if (answer == "bye") {
            std::cout << "Exiting...\n" << std::endl;
            break;
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

Packet & ConnectionHandler::getLine() {
    char ch;
    string line;
    // Stop when we encounter the null character.
    // Notice that the null character is not appended to the frame string.
    try {
        for (int i = 0; i < 2; i++) {
            getBytes(&ch, 1);
            line.append(1, ch);
        }
        short opCode = bytesToShort((char *) line.c_str());
        switch (opCode)
            case 3:
                    //data
            case 4:
                //ack
            case 5:
            case 9:
                //until 0


    } catch (std::exception &e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
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

// Close down the connection properly.
void ConnectionHandler::close() {
    try {
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
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