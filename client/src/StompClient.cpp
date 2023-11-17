#include <stdlib.h>
#include <thread>
#include "../include/ConnectionHandler.h"
#include <string>
using namespace std;
using std::string;
class StompClient;
#include <vector>
#include <iostream>
#include <sstream>
#include "StompClient.h"
class StompProtocol;
#include "StompProtocol.h"
#include "event.h"

std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

void keyboardTask(ConnectionHandler *connectionHandler, StompProtocol *stompProtocol)
{

    while (1)
    {
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
        std::string line(buf);
        // int len = line.length();
        vector<string> splitMessage = split(line, ' ');


        string commandName = splitMessage[0];

        if (commandName != "login" && !connectionHandler->isConnected)
            std::cerr << "log in first " << std::endl;

        else
        {
            if (commandName == "login")
            {
                if (connectionHandler->isConnected)
                {
                    std::cerr << " client already logged-in " << std::endl;
                    
                }
                else
                {
                    
                    vector<string> SplitedHostandPort = split(splitMessage[1], ':');
                    string host = SplitedHostandPort[0];
                    int port = std::stoi(SplitedHostandPort[1]);
                    connectionHandler->setHostAndPort(port, host);
                    connectionHandler->isConnected = true;
                    if (!connectionHandler->connect())
                    {
                        connectionHandler->isConnected = false;
                        std::cerr << "Cannot connect to server" << std::endl;
                    }
                else {//creates new connection
                    stompProtocol->ShouldRunSocker=true;

                }
                }
            }

            string toSend = stompProtocol->proccessCommand(line);
          
            if (toSend != "error")
                if (!connectionHandler->sendLine(toSend))
                {
                    std::cout << "Disconnected. Exiting...\n"
                              << std::endl;
                    break;
                }
        }
    }
}

void socketTask(ConnectionHandler *connectionHandler,StompProtocol *stompProtocol)
{
    while (1)
    {
        if(stompProtocol->ShouldRunSocker){
        std::string answer;
        if (!connectionHandler->getLine(answer))
        {
            std::cout << "Disconnected. Exiting...\n"
                      << std::endl;
            break;
        }
        stompProtocol->proccessInputFrame(answer); //  segamention fault 

        }
    }
}

/**
 * This code assumes that the server replies the exact text the client sent it (as opposed to the practical session example)
 */
int main()
{

    ConnectionHandler *connectionHandler = new ConnectionHandler();

    StompProtocol *stompProtocol = new StompProtocol(connectionHandler);

    const short bufsize = 1024;
    char buf[bufsize];
    std::cin.getline(buf, bufsize);
    std::string line(buf);
    vector<string> splitMessage = split(line, ' ');
    string commandName = splitMessage[0];

    if (commandName != "login")
        std::cerr << "logged-in  first " << std::endl;

    else
    {
        vector<string> SplitedHostandPort = split(splitMessage[1], ':');
        string host = SplitedHostandPort[0];
        int port = std::stoi(SplitedHostandPort[1]);
        connectionHandler->setHostAndPort(port, host);
        connectionHandler->isConnected = true;
        if (!connectionHandler->connect())
        {
            connectionHandler->isConnected = false;
            std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
        }

        string toSend = stompProtocol->proccessCommand(line);
        if (!connectionHandler->sendLine(toSend))
        {
            std::cout << "Disconnected. Exiting...\n"
                      << std::endl;
        }

        std::thread socketThread(socketTask, connectionHandler,stompProtocol);
        std::thread keyboardThread(keyboardTask, connectionHandler, stompProtocol);

        keyboardThread.join();
        socketThread.join();

        return 0;
    }
}
