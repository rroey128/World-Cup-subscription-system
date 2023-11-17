#include "../include/event.h"
#include "../include/json.hpp"
#include <iostream>
#include <fstream>
#include <queue>

#include <string>
#include <map>
#include <vector>
#include <sstream>
using json = nlohmann::json;
using namespace std;
#include "event.h"




Event::Event(std::string team_a_name, std::string team_b_name, std::string name, int time,
             std::map<std::string, std::string> game_updates, std::map<std::string, std::string> team_a_updates,
             std::map<std::string, std::string> team_b_updates, std::string discription)
    : team_a_name(team_a_name), team_b_name(team_b_name), name(name),
      time(time), game_updates(game_updates), team_a_updates(team_a_updates),
      team_b_updates(team_b_updates), description(discription)
{
}

Event::~Event()
{
}

const std::string &Event::get_team_a_name() const
{
    return this->team_a_name;
}

const std::string &Event::get_team_b_name() const
{
    return this->team_b_name;
}

const std::string &Event::get_name() const
{
    return this->name;
}

int Event::get_time() const
{
    return this->time;
}

const std::map<std::string, std::string> &Event::get_game_updates() const
{
    return this->game_updates;
}

const std::map<std::string, std::string> &Event::get_team_a_updates() const
{
    return this->team_a_updates;
}

const std::map<std::string, std::string> &Event::get_team_b_updates() const
{
    return this->team_b_updates;
}

const std::string &Event::get_discription() const
{
    return this->description;
}


std::vector<std::string> split4(const std::string &s, char delimiter)
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


Event::Event(const std::string &frame_body) : team_a_name(""), team_b_name(""), name(""), time(0), game_updates(), team_a_updates(), team_b_updates(), description("")
{

vector <string> splittedframe= split4(frame_body,'\n');
for (unsigned int i=0; i<splittedframe.size(); i++){
    if (splittedframe[i].find("team a: ") != std::string::npos){
        team_a_name = splittedframe[i].substr(8);
    }
    if (splittedframe[i].find("team b: ") != std::string::npos){
        team_b_name = splittedframe[i].substr(8);
    }
    if (splittedframe[i].find("event name: ") != std::string::npos){
        name = splittedframe[i].substr(12);
    }
    if (splittedframe[i].find("time: ") != std::string::npos){
        if(!(splittedframe[i].find("halftime") != std::string::npos)){
    string Time = splittedframe[i].substr(6);
        time = std::stoi(Time);
    }
    }
    if (splittedframe[i].find("general game updates: ") != std::string::npos){
        int j=i+1;
        while (splittedframe[j].find("team a updates: ") == std::string::npos){
            vector <string> splittedupdate= split4(splittedframe[j],':');
            game_updates[splittedupdate[0]] = splittedupdate[1];
            j++;
        }
    }
    if (splittedframe[i].find("team a updates: ") != std::string::npos){
        int j=i+1;
        while (splittedframe[j].find("team b updates: ") == std::string::npos){
            vector <string> splittedupdate= split4(splittedframe[j],':');
            team_a_updates[splittedupdate[0]] = splittedupdate[1];
            j++;
        }
    }
    if (splittedframe[i].find("team b updates: ") != std::string::npos){
        int j=i+1;
        while (splittedframe[j].find("description: ") == std::string::npos){
            vector <string> splittedupdate= split4(splittedframe[j],':');
            team_b_updates[splittedupdate[0]] = splittedupdate[1];
            j++;
        }
    }
    if (splittedframe[i].find("description: ") != std::string::npos){
        description = splittedframe[i].substr(13);
    }
}

}




string Event::eventToBody(string username){
    
    string gameName = team_a_name+"_"+team_b_name;
    string body = "user: "+username+"\n"
    +"team a: "+team_a_name+"\n"
    +"team b: "+team_b_name+"\n"
    +"event name: "+name+"\n"
    +"time: "+to_string(time)+"\n"
    +"general game updates: "+"\n";
    
    for (auto &update : game_updates)
    {
        body += update.first + ": " + update.second + "\n";
    }

    body += "team a updates: \n";
    for(auto &update : team_a_updates)
    {
        body += update.first + ": " + update.second + "\n";
    }
    
    body += "team b updates: \n";
    for(auto &update : team_b_updates)
    {
        body += update.first + ": " + update.second + "\n";
    }
    body += "description: " + description + "\n";




    string commandToSend = "SEND\ndestination:/"+gameName +"\n\n"+body+"\n\0";

    return commandToSend;
}

names_and_events parseEventsFile(std::string json_path)
{
    std::ifstream f(json_path);
    json data = json::parse(f);

    std::string team_a_name = data["team a"];
    std::string team_b_name = data["team b"];

    // run over all the events and convert them to Event objects
    std::vector<Event> events;
    for (auto &event : data["events"])
    {
        std::string name = event["event name"];
        int time = event["time"];
        std::string description = event["description"];
        std::map<std::string, std::string> game_updates;
        std::map<std::string, std::string> team_a_updates;
        std::map<std::string, std::string> team_b_updates;
        for (auto &update : event["general game updates"].items())
        {
            if (update.value().is_string())
                game_updates[update.key()] = update.value();
            else
                game_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team a updates"].items())
        {
            if (update.value().is_string())
                team_a_updates[update.key()] = update.value();
            else
                team_a_updates[update.key()] = update.value().dump();
        }

        for (auto &update : event["team b updates"].items())
        {
            if (update.value().is_string())
                team_b_updates[update.key()] = update.value();
            else
                team_b_updates[update.key()] = update.value().dump();
        }
        
        events.push_back(Event(team_a_name, team_b_name, name, time, game_updates, team_a_updates, team_b_updates, description));
    }
    names_and_events events_and_names{team_a_name, team_b_name, events};

    return events_and_names;
}