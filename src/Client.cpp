#include <stdlib.h>
#include <boost/thread.hpp>
#include "Client.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);

    ConnectionHandler connectionHandler(host, port);
    if (!connectionHandler.connect()) {
        std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        return 1;
    }

    boost::thread serverThread(&ConnectionHandler::run);

    while (true) {
        std::string line;
        std::getline(std::cin, line);
        string ans = Client::checkFunction(line);
        if (ans.compare("OK")) {
            char *message = connectionHandler.encode(Client::stringToPacket(line));
            connectionHandler.insertToQueue(message);
        } else
            std::cout << ans + "\n" << std::endl;
    }
    return 0;
}

static string Client::checkFunction(string &line) {
    if (line == nullptr || line == "") return "Wrong function";
    string delimiter = " ";
    string functionName = line.substr(0, line.find(delimiter));
    std::transform(functionName.begin(), functionName.end(), functionName.begin(), ::toupper);
    string file_user_name = line.substr(line.find(delimiter), line.length());;
    if (functionName.compare("RRQ") || functionName.compare("WRQ") || functionName.compare("DELRQ")) {
        if (file_user_name.find("0")) return "Invalid character: Filename contains 0";
        else return "OK";
    } else if (functionName.compare("LOGRQ")) {
        if (file_user_name.find("0")) return "Invalid character: Username contains 0";
        else return "OK";
    } else if (functionName.compare("DISC") || functionName.compare("DIRQ")) {
        if (trim(file_user_name) == "") return "OK";
        else return "Wrong function";
    } else
        return "Wrong function";
}

static string Client::trim(const string &str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

static Packet* Client::stringToPacket(string &line) {
    string delimiter = " ";
    string functionName = line.substr(0, line.find(delimiter));
    std::transform(functionName.begin(), functionName.end(), functionName.begin(), ::toupper);
    string file_user_name = line.substr(line.find(delimiter), line.length());;
    if (functionName.compare("RRQ")) {
        return new RRQ(file_user_name);
    } else if (functionName.compare("WRQ")) {
        return new WRQ(file_user_name);
    } else if (functionName.compare("DELRQ")) {
        return new DELRQ(file_user_name);
    } else if (functionName.compare("LOGRQ")) {
        return new LOGRQ(file_user_name);
    } else if (functionName.compare("DISC")) {
        return new DISC();
    } else { //DIRQ
        return new DIRQ();
    }
}