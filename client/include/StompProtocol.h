#pragma once

#include "../include/ConnectionHandler.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;
#include <iostream>
#include <map>
class Game;
//#include <concurrent_map>




class StompProtocol
{
private:

int SubID;
int receipt;
string userName;
ConnectionHandler* connectionHandlerr;


public:

bool ShouldRunSocker;
map<std::string, int> TopicToSubID;
map<string, Game> TopicToGame;
map<int, string> ReceiptIDtoCommand;

StompProtocol(const StompProtocol& other);
StompProtocol& operator=(const StompProtocol& other);
StompProtocol(ConnectionHandler* connectionHandler);
bool CheckIfSubbed(string gameName);
void clearProtocol();
string proccessCommand(string command);
void proccessInputFrame(string Frame);
string findString(string lookingFor, vector<string> Frame);
}
;
