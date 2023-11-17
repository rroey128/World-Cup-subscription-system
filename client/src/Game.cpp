#include <stdlib.h>
#include <thread>
#include "../include/ConnectionHandler.h"
#include <string>
using namespace std;
using std::string;
class StompClient;
#include <vector>
#include <iostream>
#include "StompProtocol.h"
#include "Game.h"
#include "event.h"
#include <queue>
class Event;



Game::Game() : gameName(), usernameToEvents() 
{
}

void Game::updateUsernameToEvents(queue<Event> firstHalf, queue<Event> secondHalf, string username)
{
    pair<queue<Event>, queue<Event>> Pair;
    Pair.first = firstHalf;
    Pair.second = secondHalf;
    usernameToEvents[username] = Pair;
}

void Game::setUsernameToEvents(queue<Event> firstHalf, queue<Event> secondHalf, string username)
{
    pair<queue<Event>, queue<Event>> Pair;
    Pair.first = firstHalf;
    Pair.second = secondHalf;

    usernameToEvents[username] = Pair;
}

std::vector<std::string> split3(const std::string &s, char delimiter)
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

void Game::setGameName(string gameNameToPut){

    gameName = gameNameToPut;
}

string Game::getSummary(string username)
{
    queue<Event> firstHalf = usernameToEvents[username].first;
    queue<Event> secondHalf = usernameToEvents[username].second;
    string summary = "";
    vector<string> names = split3(gameName, '_');
    string gameHeader = names[0] + " vs " + names[1];
    string gameStatsHeader = "Game stats:";
    string stats = getStats2(username);
    string GameEventsReportHeader = "Game event reports: ";
    string reports = "";
    while (!firstHalf.empty())
    {
        Event event = firstHalf.front();
        firstHalf.pop();
        reports += std::to_string(event.get_time()) + " - " + event.get_name() + ":" + "\n\n";
        reports += event.get_discription() + "\n\n";
    }
    while (!secondHalf.empty())
    {
        Event event = secondHalf.front();
        secondHalf.pop();
        reports += std::to_string(event.get_time()) + " - " + event.get_name() + ":" + "\n\n";
        reports += event.get_discription() + "\n\n";
    }
    summary = gameHeader + "\n" + gameStatsHeader + "\n" + stats + "\n\n" + GameEventsReportHeader + "\n\n" + reports;
    return summary;
}


string Game::getStats2(string username)
{
    queue<Event> firstHalf = usernameToEvents[username].first;
    queue<Event> secondHalf = usernameToEvents[username].second;
    string teamAName = split3(gameName, '_')[0];
    string teamBName = split3(gameName, '_')[1];
    string stats = "";
    string generalStatsHeader = "general stats:";
    string teamAStatsHeader = teamAName + " stats:";
    string teamBStatsHeader = teamBName + " stats:";
    map<string, string> teamAStats;
    map<string, string> teamBStats;
    map<string, string> generalUpdates;

    while (!firstHalf.empty())
    {
        Event event = firstHalf.front();
        firstHalf.pop();
        map<std::string, std::string> gameUpdates = event.get_game_updates();
        map<std::string, std::string> teamA = event.get_team_a_updates();
        map<std::string, std::string> teamB = event.get_team_b_updates();

        for (auto gameUpdate : gameUpdates)
        {
            generalUpdates[gameUpdate.first] = gameUpdate.second;
        }
        for (auto teamAUpdate : teamA)
        {
            teamAStats[teamAUpdate.first] = teamAUpdate.second;
        }
        for (auto teamBUpdate : teamB)
        {
            teamBStats[teamBUpdate.first] = teamBUpdate.second;
        }
    }

    while (!secondHalf.empty())
    {
        Event event = secondHalf.front();
        secondHalf.pop();
        map<std::string, std::string> gameUpdates = event.get_game_updates();
        map<std::string, std::string> teamA = event.get_team_a_updates();
        map<std::string, std::string> teamB = event.get_team_b_updates();

        for (auto gameUpdate : gameUpdates)
            generalUpdates[gameUpdate.first] = gameUpdate.second;
        for (auto teamAUpdate : teamA)
            teamAStats[teamAUpdate.first] = teamAUpdate.second;
        for (auto teamBUpdate : teamB)
            teamBStats[teamBUpdate.first] = teamBUpdate.second;
    }

    stats += generalStatsHeader + "\n";
    vector<string> generalUpdatesKeys;
    for (auto generalUpdate : generalUpdates)
    {
        generalUpdatesKeys.push_back(generalUpdate.first);
    }
    std::sort(generalUpdatesKeys.begin(), generalUpdatesKeys.end());

    for (auto generalUpdateKey : generalUpdatesKeys)
    {
        stats += generalUpdateKey + ": " + generalUpdates[generalUpdateKey] + "\n";
    }
    stats += teamAStatsHeader + "\n";
    vector<string> teamAStatsKeys;
    for (auto teamAStat : teamAStats)
    {
        teamAStatsKeys.push_back(teamAStat.first);
    }
    std::sort(teamAStatsKeys.begin(), teamAStatsKeys.end());

    for (auto teamAStatKey : teamAStatsKeys)
    {
        stats += teamAStatKey + ": " + teamAStats[teamAStatKey] + "\n";
    }
    stats += teamBStatsHeader + "\n";
    vector<string> teamBStatsKeys;
    for (auto teamBStat : teamBStats)
    {
        teamBStatsKeys.push_back(teamBStat.first);
    }
    std::sort(teamBStatsKeys.begin(), teamBStatsKeys.end());

    for (auto teamBStatKey : teamBStatsKeys)
    {
        stats += teamBStatKey + ": " + teamBStats[teamBStatKey] + "\n";
    }


    return stats;
}


