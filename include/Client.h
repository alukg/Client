#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include <packets/Packet.h>
#include "ConnectionHandler.h"

using std::string;

class Client {
private:
    static string trim(const string& str);
public:
    static string checkFunction(string &line);
    static Packet* stringToPacket(string &line);
};

#endif //CLIENT_CLIENT_H
