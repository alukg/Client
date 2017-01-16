#ifndef CONNECTION_HANDLER__
#define CONNECTION_HANDLER__

#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include "NonBlockingQueue.h"

using boost::asio::ip::tcp;

class ConnectionHandler {
private:
    const std::string host_;
    const short port_;
    boost::asio::io_service io_service_;   // Provides core I/O functionality
    tcp::socket socket_;
    NonBlockingQueue sendToServerQueue;
    queue<DATA> dataForSendQueue;
    Packet* lastPacketISent;
    char* gettingData;
    char* readFileBytes(const char *name);
    void convertDataToPackets(char *data);

public:
    ConnectionHandler(std::string host, short port);
    ConnectionHandler(const ConnectionHandler& old);
    Packet* process(Packet &packet);
    virtual ~ConnectionHandler();

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

    char* encode(Packet &packet);

    void insertToQueue(Packet &message);

    bool sendPacket(Packet packet);

    // Close down the connection properly.
    void close();

    short bytesToShort(char* bytesArr);
    void shortToBytes(short num, char* bytesArr);

    char* connectArrays(char *firstArr, char *secondArr);

}; //class ConnectionHandler

#endif