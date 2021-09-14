#ifndef CAMPAIGN_H
#define CAMPAIGN_H

#include <list>
#include <map>
#include <string>

class Player;
class Location;

class Campaign
{
public:

    Campaign();
    ~Campaign();

    int rollDie(int d);

    Player* addPlayer(std::string name, int initiative, int xloc, int yloc);
//    Player *getPlayer(int at);

    void startTurn();
    void endTurn();

    Location *getLocation(int xloc, int yloc);
    void sortPlayers(std::list<Player*> &list);

private:
    std::list<Player*> m_players;
    std::map<int, std::map<int, Location*> *> m_map;
};

#endif // CAMPAIGN_H
