#include <stdlib.h>
#include <thread>
#include "../include/ConnectionHandler.h"
#include <string>
using namespace std;
using std::string;
class StompClient;
#include <vector>
#include <iostream>
using namespace std;
#include <iostream>
#include <map>
class Game;
#include <queue>
class Event;
using namespace std;


class Game{


private :
 





public : 
Game();

string gameName;
std::map<std::string, std::pair<queue<Event>, queue<Event>>> usernameToEvents;
void updateUsernameToEvents(queue<Event> firstHalf, queue<Event> secondHalf, string username);
void setUsernameToEvents(queue<Event> firstHalf, queue<Event> secondHalf, string username);
string getSummary(string username);
string getActiveHeader(string username);
string getBeforeHalfTimeHeader(string username);
string getStats2(string username);
void setGameName(string gameNameToPut);


};


