#include <stdlib.h>
#include <boost/thread.hpp>
#include <fstream>
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
    cout << "connection succeed" << endl;
    boost::thread serverThread(boost::bind(&ConnectionHandler::run, &connectionHandler));

    while (true) {
        std::string line;
        std::getline(std::cin, line);
        string ans = Client::checkFunction(line);
        if (ans.compare("OK") == 0) {
            Packet* lastPacketISent = connectionHandler.getLastPacketISent();
            delete lastPacketISent;
            Packet *packet = Client::stringToPacket(line);
            if (!connectionHandler.sendPacket(packet, true)) {
                std::cout << "Disconnected. Exiting..." << std::endl;
                break;
            }
            if (packet->getOpCode() == 10)
                break;
        } else
            std::cout << ans << std::endl;
    }
    serverThread.join();
    return 0;
}

string Client::checkFunction(string &line) {
    if (line == "") return "Wrong function";
    string delimiter = " ";
    string functionName = line.substr(0, line.find(delimiter));
    std::transform(functionName.begin(), functionName.end(), functionName.begin(), ::toupper);
    string file_user_name = "";
    if (functionName.length() < line.length()) {
        file_user_name = line.substr(functionName.length(), line.length());
        trim(file_user_name);
    }
    if (functionName.compare("RRQ") == 0 || functionName.compare("WRQ") == 0 || functionName.compare("DELRQ") == 0) {

        if (file_user_name == "") return "No file name entered.";
        else {
            if (functionName.compare("WRQ") == 0) {
                ifstream fstream(file_user_name);
                if (!fstream.is_open())
                    return "Unable to open file";
            }
            return "OK";
        }
    } else if (functionName.compare("LOGRQ") == 0) {
        if (file_user_name == "") return "No username entered.";
        else return "OK";
    } else if (functionName.compare("DISC") == 0 || functionName.compare("DIRQ") == 0) {
        if (file_user_name == "") return "OK";
        else return "Wrong function";
    } else
        return "Wrong function";
}

void Client::trim(string &str) {
    int first = str.find_first_not_of(' ');
    if (string::npos != first) {
        int last = str.find_last_not_of(' ');
        str = str.substr(first, (last - first + 1));
    }
}

Packet *Client::stringToPacket(string &line) {
    string delimiter = " ";
    string functionName = line.substr(0, line.find(delimiter));
    std::transform(functionName.begin(), functionName.end(), functionName.begin(), ::toupper);
    string file_user_name = "";
    if (functionName.length() < line.length()) {
        file_user_name = line.substr(functionName.length(), line.length());
        trim(file_user_name);
    }
    if (functionName.compare("RRQ") == 0) {
        return new RRQ(file_user_name);
    } else if (functionName.compare("WRQ") == 0) {
        return new WRQ(file_user_name);
    } else if (functionName.compare("DELRQ") == 0) {
        return new DELRQ(file_user_name);
    } else if (functionName.compare("LOGRQ") == 0) {
        return new LOGRQ(file_user_name);
    } else if (functionName.compare("DISC") == 0) {
        return new DISC();
    } else { //DIRQ
        return new DIRQ();
    }
}