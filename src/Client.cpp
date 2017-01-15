#include <stdlib.h>
#include <boost/thread.hpp>
#include "Client.h"
#include "ConnectionHandler.h"

int main (int argc, char *argv[]) {
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
        std::getline (std::cin,line);
        string ans = Client::checkFunction(line);
        if (ans.compare("OK")){
            connectionHandler.insertToQueue(line);
        } else
            std::cout << ans + "\n" << std::endl;

        if (!connectionHandler.sendLine(line)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }

        //std::cout << "Sent " << len+1 << " bytes to server" << std::endl;


        // We can use one of three options to read data from the server:
        // 1. Read a fixed number of characters
        // 2. Read a line (up to the newline character using the getline() buffered reader
        // 3. Read up to the null character
        std::string answer;
        // Get back an answer: by using the expected number of bytes (len bytes + newline delimiter)
        // We could also use: connectionHandler.getline(answer) and then get the answer without the newline char at the end
        if (!connectionHandler.getLine(answer)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
            break;
        }

        len=answer.length();
        // A C string must end with a 0 char delimiter.  When we filled the answer buffer from the socket
        // we filled up to the \n char - we must make sure now that a 0 char is also present. So we truncate last character.
        answer.resize(len-1);
        std::cout << "Reply: " << answer << " " << len << " bytes " << std::endl << std::endl;
        if (answer == "bye") {
            std::cout << "Exiting...\n" << std::endl;
            break;
        }
    }
    return 0;
}

static string Client::checkFunction(string &line) {
    if(line == nullptr || line == "") return "Wrong function";
    string delimiter = " ";
    string functionName = line.substr(0, line.find(delimiter));
    std::transform(functionName.begin(), functionName.end(), functionName.begin(), ::toupper);
    string file_user_name = line.substr(line.find(delimiter),line.length());;
    if(functionName.compare("RRQ") || functionName.compare("WRQ") || functionName.compare("DELRQ")) {
        if (file_user_name.find("0")) return "Invalid character: Filename contains 0";
        else return "OK";
    }
    else if(functionName.compare("LOGRQ")) {
        if(file_user_name.find("0")) return "Invalid character: Username contains 0";
        else return "OK";
    }
    else if(functionName.compare("DISC") || functionName.compare("DIRQ")) {
        if(trim(file_user_name) == "") return "OK";
        else return "Wrong function";
    } else
        return "Wrong function";
}

static string Client::trim(const string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}