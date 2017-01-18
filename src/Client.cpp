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
    cout << "connection succeed" << endl;
    boost::thread serverThread(boost::bind(&ConnectionHandler::run, &connectionHandler));

    while (true) {
        std::string line;
        std::getline(std::cin, line);
        string ans = Client::checkFunction(line);
        if (ans.compare("OK") == 0) {
            Packet *packet = Client::stringToPacket(line);
            if (!connectionHandler.sendPacket(packet, true)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
            if(packet->getOpCode()==10)
                break;
        } else
            std::cout << ans + "\n" << std::endl;
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
        file_user_name = trim(file_user_name);
    }
    if (functionName.compare("RRQ") == 0 || functionName.compare("WRQ") == 0 || functionName.compare("DELRQ") == 0) {
        if (file_user_name.find("0") != std::string::npos) return "Invalid character: Filename contains 0";
        else if (file_user_name == "") return "No file name entered.";
        else return "OK";
    } else if (functionName.compare("LOGRQ") == 0) {
        if (file_user_name.find("0") != std::string::npos) return "Invalid character: Username contains 0";
        else if (file_user_name == "") return "No username entered.";
        else return "OK";
    } else if (functionName.compare("DISC") == 0 || functionName.compare("DIRQ") == 0) {
        if (file_user_name == "") return "OK";
        else return "Wrong function";
    } else
        return "Wrong function";
}

string Client::trim(const string &str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

Packet *Client::stringToPacket(string &line) {
    string delimiter = " ";
    string functionName = line.substr(0, line.find(delimiter));
    std::transform(functionName.begin(), functionName.end(), functionName.begin(), ::toupper);
    string file_user_name = "";
    if (functionName.length() < line.length()) {
        file_user_name = line.substr(functionName.length(), line.length());
        file_user_name = trim(file_user_name);
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