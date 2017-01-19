#ifndef CONNECTION_HANDLER__
#define CONNECTION_HANDLER__

#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <queue>

using boost::asio::ip::tcp;

class ConnectionHandler {
private:
    const std::string host_;
    const short port_;
    boost::asio::io_service io_service_;   // Provides core I/O functionality
    tcp::socket socket_;
    queue<DATA*> dataForSendQueue;
    Packet* lastPacketISent;
    char* gettingData = nullptr;
    int gettingDataSize;
    bool shouldTerminate;
    char* readFileBytes(char const* filename, int &arrSize);
    void convertDataToPackets(char *data, int len);

public:
    ConnectionHandler(std::string host, short port);
    Packet* process(Packet &packet);
    virtual ~ConnectionHandler();

    Packet* getLastPacketISent();

    // Connect to the remote machine
    bool connect();

    void run();

    // Read a fixed number of bytes from the server - blocking.
    // Returns false in case the connection is closed before bytesToRead bytes can be read.
    bool getBytes(char bytes[], unsigned int bytesToRead);

    // Send a fixed number of bytes from the client - blocking.
    // Returns false in case the connection is closed before all the data is sent.
    bool sendBytes(const char bytes[], int bytesToWrite);

    // Read an ascii line from the server
    // Returns false in case connection closed before a newline can be read.
    Packet * getLine();

    void encode(Packet* packet, char* encodedArr);

    bool sendPacket(Packet* packet, bool save);

    // Close down the connection properly.
    void close();

    short bytesToShort(char* bytesArr);
    void shortToBytes(short num, char* bytesArr);

    void connectArrays(const char *firstArr, int firstLength,const char *secondArr, int secondLength, char *connected);

}; //class ConnectionHandler

#endif