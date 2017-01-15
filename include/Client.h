#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

#include "ConnectionHandler.h"

using std::string;

class Client {
private:
    static string trim(const string& str);
public:
    static string checkFunction(string &line);

};

#endif //CLIENT_CLIENT_H
