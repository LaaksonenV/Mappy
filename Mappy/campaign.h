#ifndef CAMPAIGN_H
#define CAMPAIGN_H

#include <list>
#include <vector>
#include <map>
#include <string>

class Player;
class Location;

class Campaign
{
public:

    enum PlayerData
    {
        e_null
        , e_Initiative
        , e_Name
        , e_Location
        , e_Camp
        , e_Fight
        , e_Moves
    };

    Campaign();
    ~Campaign();

    int rollDie(int d);

    int addPlayer();
    bool removePlayer(int id);
    std::string getPlayerData(int id, int type);
    bool setPlayerData(int id, int type, std::string data);

    bool startTurn();
    bool endTurn();

    Location *getLocation(std::string id);
    void sortPlayers(std::list<Player*> &list);

private:
    Player *getPlayer(int at);

    std::vector<Player*> m_players;
    std::map<int, std::map<int, Location*> *> m_map;
};

#endif // CAMPAIGN_H
