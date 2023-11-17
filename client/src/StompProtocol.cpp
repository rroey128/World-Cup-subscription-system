#include <stdlib.h>
#include <thread>
#include "../include/ConnectionHandler.h"
#include <string>
using std::string;
using std::vector;
#include <vector>
#include "StompProtocol.h"
#include <iostream>
#include <map>
#include "event.h"
class Game;
#include "Game.h"
class event;
class ConnectionHandler;
#include <queue>
#include <fstream>


StompProtocol::StompProtocol(ConnectionHandler *connectionHandler)
    : SubID(0), receipt(0), userName(),  connectionHandlerr(connectionHandler), ShouldRunSocker(true), TopicToSubID(), TopicToGame(), ReceiptIDtoCommand()
{
}

StompProtocol::StompProtocol(const StompProtocol& other) : SubID(other.SubID),  receipt(other.receipt),userName(other.userName), 
connectionHandlerr(other.connectionHandlerr) ,ShouldRunSocker(other.ShouldRunSocker),  TopicToSubID(other.TopicToSubID),TopicToGame(other.TopicToGame),
ReceiptIDtoCommand(other.ReceiptIDtoCommand) {
}


StompProtocol& StompProtocol::operator=(const StompProtocol& other) {
    if (this == &other) return *this; 
    SubID = other.SubID;
    receipt = other.receipt;
    ShouldRunSocker = other.ShouldRunSocker;
    TopicToSubID = other.TopicToSubID;
    TopicToGame = other.TopicToGame;
    ReceiptIDtoCommand = other.ReceiptIDtoCommand;
    userName = other.userName;
    connectionHandlerr = other.connectionHandlerr;
    return *this;
}



void StompProtocol::clearProtocol()
{
    SubID = 0;
    receipt = 0;
    ShouldRunSocker = true;
    TopicToSubID.clear();
    TopicToGame.clear();
    ReceiptIDtoCommand.clear();
    userName = "";
}

bool contains(string full, string shorter)
{
    bool found = full.find(shorter) != std::string::npos;
    return found;
}

std::vector<std::string> split1(const std::string &s, char delimiter)
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

string StompProtocol::proccessCommand(string command)
{

    vector<string> splitMessage = split1(command, ' ');
    string commandName = splitMessage[0];

    if (commandName == "login")
    {
        userName = splitMessage[2];
        vector<string> SplitedHostandPort = split1(splitMessage[1], ':');
        return "CONNECT\naccept-version:1.2\nhost:" + SplitedHostandPort[0] + "\nlogin:" + splitMessage[2] + "\npasscode:" + splitMessage[3] + "\n\n\0";
    }

    else if (commandName == "join")
    {
        receipt++;
        SubID++;
        string destination = splitMessage[1];
        TopicToSubID[destination] = SubID;
        
        ReceiptIDtoCommand[receipt] = "SUBSCRIBE:" + destination;
        return "SUBSCRIBE\ndestination:/" + destination + "\nid:" + std::to_string(SubID) + "\nreceipt:" + std::to_string(receipt) + "\n\n\0";
    }

    else if (commandName == "exit")
    {
        
        receipt++;
        string destination = splitMessage[1];
          if (!CheckIfSubbed(destination))
        {
            std::cout << "You are not subscribed to" + destination << std::endl;

            return "error";
        }
        
        int SubIDtoUnsub = TopicToSubID[destination];
        ReceiptIDtoCommand[receipt] = "UNSUBSCRIBE:" + destination;

        TopicToSubID.erase(destination);

        return "UNSUBSCRIBE\nid:" + std::to_string(SubIDtoUnsub) + "\nreceipt:" + std::to_string(receipt) + "\n\n\0";
    }

    else if (commandName == "report")
    {

        bool beforeHalfTime = true;
        string filePath = splitMessage[1];                                   // saves the path to the file recieved from the keyboard
        names_and_events myEvents = parseEventsFile(filePath);               // parse the file to names_and_events struct
        string gameName = myEvents.team_a_name + "_" + myEvents.team_b_name; // creates the game name
        if (!CheckIfSubbed(gameName))
        {

            std::cout << "You are not subscribed to : "  + gameName +"please subscribe first before reporting events" << std::endl;

            return "error";
        }
        vector<Event> reportedEvents = myEvents.events; // get the vector of reported events
        queue<Event> firstHalf;
        queue<Event> secondHalf;
        string SendFrameEvent;
        if (TopicToGame.count(gameName)) // checks if this game already exists
        {                                // if it does, add the events to the game under this user
            Game currentGame = TopicToGame[gameName];
            if (currentGame.usernameToEvents.count(userName))
            { // this user reported on this game in past
                firstHalf = currentGame.usernameToEvents[userName].first;
                secondHalf = currentGame.usernameToEvents[userName].second;

                for (Event event : reportedEvents)
                {
                    if (event.get_game_updates().count("before halftime") == true)
                    {
                        if (event.get_game_updates().at("before halftime") == "true")
                            beforeHalfTime = true;
                        else
                            beforeHalfTime = false;
                    }

                    if (beforeHalfTime == true)
                        firstHalf.push(event);

                    else
                        secondHalf.push(event);

                    SendFrameEvent = event.eventToBody(userName); // send this via connection handler
                    connectionHandlerr->sendLine(SendFrameEvent);
                }
                currentGame.updateUsernameToEvents(firstHalf, secondHalf, userName);
                TopicToGame[gameName] = currentGame;
            }

            else
            { // this user never reported on this game in past

                for (Event event : reportedEvents)
                {
                    if (event.get_game_updates().count("before halftime") == true)
                    {
                        if (event.get_game_updates().at("before halftime") == "true")
                            beforeHalfTime = true;
                        else
                            beforeHalfTime = false;
                    }

                    if (beforeHalfTime == true)
                        firstHalf.push(event);

                    else
                        secondHalf.push(event);

                    SendFrameEvent = event.eventToBody(userName); // send this via connection handler
                    connectionHandlerr->sendLine(SendFrameEvent);
                }

                currentGame.setUsernameToEvents(firstHalf, secondHalf, userName);
                TopicToGame[gameName] = currentGame;
            }
        }

        else // if it doesn't, create a new game and add the events to it under this user
        {

            Game newGame;

            for (Event event : reportedEvents)
            {
                if (event.get_game_updates().count("before halftime") == true)
                {
                    if (event.get_game_updates().at("before halftime") == "true")
                        beforeHalfTime = true;
                    else
                        beforeHalfTime = false;
                }

                if (beforeHalfTime == true)
                    firstHalf.push(event);

                else
                    secondHalf.push(event);

                SendFrameEvent = event.eventToBody(userName); // send this via connection handler
                connectionHandlerr->sendLine(SendFrameEvent);
            }
            newGame.setUsernameToEvents(firstHalf, secondHalf, userName);
            TopicToGame[gameName] = newGame;
        }

        return "error";
    }

    else if (commandName == "summary")
    {

        string gameName = splitMessage[1];

        if (!CheckIfSubbed(gameName))
        {

            std::cout << "You are not subscribed to "+ gameName  +" please subscribe first before getting a summary" << std::endl;

            return "error";
        }
        string user = splitMessage[2];
        string filePath = splitMessage[3];
        Game currentGame = TopicToGame[gameName];
        currentGame.setGameName(gameName);
        string summary = currentGame.getSummary(user);
        std::ofstream file(filePath, std::ios::trunc);
        file << summary;
        file.close();
        return "error";
    }

    else if (commandName == "logout")
    {
        receipt++;
        ReceiptIDtoCommand[receipt] = "DISCONNECT: ";
        return "DISCONNECT\nreceipt:" + std::to_string(receipt) + "\n\n\0";
    }
    else
    {
        return "invaild Command \n\n\0";
    }
}

string StompProtocol::findString(string lookingFor, vector<string> Frame)
{
    string Line = "";
    for (string line : Frame)
    {
        if (contains(line, lookingFor))
        {
            Line = line;
            break;
        }
    }
    if (Line == "")
        return Line;
    else
    {
        vector<string> splited = split1(Line, ':');
        return splited[1];
    }
}

void StompProtocol::proccessInputFrame(string Frame)
{
    if (Frame == "")
    {
        return;
    }
    vector<string> splitMessage = split1(Frame, '\n');

    string FrameCommand = splitMessage[0];

    if (FrameCommand == "CONNECTED")
    {
        connectionHandlerr->isConnected = true;
        std::cout << "Login successful" << std::endl;
    }

    else if (FrameCommand == "ERROR")
    {

        if (contains(splitMessage[1], "message"))
        {
            vector<string> splittedError = split1(splitMessage[1], ':');
            std::cout << splittedError[1] << std::endl; // message errorframe body
        }
        else if (contains(splitMessage[2], "message"))
        {
            vector<string> splittedError = split1(splitMessage[2], ':');
            std::cout << splittedError[1] << std::endl; // message errorframe body
        }
        clearProtocol();
        connectionHandlerr->isConnected = false;
        connectionHandlerr->close();
        ShouldRunSocker = false;
    }

    else if (FrameCommand == "RECEIPT")
    {
        vector<string> splitedRecepit = split1(splitMessage[1], ':');
        int receipt = stoi(splitedRecepit[1]);

        string commandReceiptFromMap = ReceiptIDtoCommand[receipt]; // check which command the receiptID is for
        vector<string> splittedReceiptCommand = split1(commandReceiptFromMap, ':');
        string command = splittedReceiptCommand[0];

        if (command == "SUBSCRIBE")
        {
            std::cout << "Joined channel " + splittedReceiptCommand[1] << std::endl;
        }
        else if (command == "UNSUBSCRIBE")
        {
            std::cout << "Exited channel " + splittedReceiptCommand[1] << std::endl;
        }

        else if (command == "DISCONNECT")
        {
            clearProtocol();
            connectionHandlerr->isConnected = false;
            connectionHandlerr->close();
            ShouldRunSocker = false;
        }

        ReceiptIDtoCommand.erase(receipt);
    }

    else if (FrameCommand == "MESSAGE")
    {
        vector<string> splittedFrame = split1(Frame, '\n');

        string user = findString("user", splittedFrame);
        user = user.substr(1);
        string gameSent = findString("destination", splittedFrame).substr(1);
        string body = Frame.substr(Frame.find("user"));
        string beforeHalf = findString("before halftime", splittedFrame);
        Event eventToAdd(body);
        Game currGame = TopicToGame[gameSent];
        queue<Event> firstHalf = currGame.usernameToEvents[user].first;
        queue<Event> secondHalf = currGame.usernameToEvents[user].second;
        if (user == userName)
            return;

        else if (!secondHalf.empty())
            secondHalf.push(eventToAdd);

        else
        {
            if (beforeHalf == "false")
                secondHalf.push(eventToAdd);
            else
                firstHalf.push(eventToAdd);
        }
        currGame.updateUsernameToEvents(firstHalf, secondHalf, user);
        TopicToGame[gameSent] = currGame;

        // parse the body to event and put in the relevant maps
        // if the user is the client himself - do not add to the relevant maps as it is being done already in the report section
    }
}
bool StompProtocol::CheckIfSubbed(string gameName)
{
    bool isSubbed = true;
    if (TopicToSubID.count(gameName) == 0)
    {
        isSubbed = false;
    }

    return isSubbed;
}
