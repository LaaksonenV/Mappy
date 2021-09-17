#include "campaign.h"

#include <stdlib.h>
#include <time.h>
//#include <map>
//#include <string>

#include "player.h"
#include "location.h"

Campaign::Campaign()
    : m_players(std::vector<Player*>())
    , m_map(std::map<int, std::map<int, Location*> *>())
{
    srand(time(NULL));
}

Campaign::~Campaign()
{
    foreach (Player *p, m_players)
    {
        delete p;
        p = nullptr;
    }
    foreach (std::map<int, Location*> *m, m_map)
    {
        foreach (Location *l, m)
        {
            delete l;
            l = nullptr;
        }
        delete m;
        m = nullptr;
    }
}

int Campaign::rollDie(int d)
{
    return rand() % d + 1;
}

int Campaign::addPlayer()
{
    int ret = (int)m_players.size();
    Player *play = new Player(std::string("Player ")+std::to_string(ret));
    m_players.push_back(play);

    return ret;
}

bool Campaign::setPlayerData(int at, std::string name, int initiative,
                             Location *loc, std::vector<Location *> moves)
{
    if (at >= m_players.size())
        return false;

    Player *play = m_players.at(at);

    play->setName(name);
    play->setInitiative(initiative);
    play->setLocation(loc);
    for (std::vector<Location*>::iterator it = moves.begin(); it != moves.end();
         ++it)
        play->addMove(*it);
}

void Campaign::startTurn()
{
    std::list<Player*> turn;
    sortPlayers(turn);

    std::list<Player*>::iterator it;
    Player *inTurn;
    Location *movement;

    while (!turn.empty())
    {
        it = turn.begin();
        while (it != turn.end())
        {
            inTurn = *it;
            movement = inTurn->pullMove();
            if (movement)
            {
                inTurn->getLocation()->moveOut(inTurn);
                movement->moveIn(inTurn);
                inTurn->setLocation(movement);
                if (movement->occupied())
                    turn.erase(it); // battle, pause movement
                else
                    ++it;
            }
            else
                turn.erase(it); // no more moves

        }
    }
}

void Campaign::endTurn()
{
    std::list<Player*> turn;
    sortPlayers(turn);

    std::list<Player*>::iterator it;
    Player *inTurn;
    Location *movement;

    while (!turn.empty())
    {
        it = turn.begin();
        while (it != turn.end())
        {
            inTurn = *it;
            movement = inTurn->pullMove();
            if (!movement || movement->occupied())
            {
                turn.erase(it);
                inTurn->clearMove();
            }
            else
            {
                ++it;
                inTurn->getLocation()->moveOut(inTurn);
                movement->moveIn(inTurn);
                inTurn->setLocation(movement);
            }
        }
    }
}

Location *Campaign::getLocation(int xloc, int yloc)
{
    std::map<Location*> *y;
    Location *loc;
    if (m_map.count(yloc))
        y = m_map.at(yloc);
    else
    {
        y = new std::map<Location*>();
        m_map.insert(yloc, y);
    }

    if (y->count(xloc))
        loc = y->at(xloc);
    else
    {
        loc = new Location();
        y->insert(xloc, loc);
    }
    return loc;
}

void Campaign::sortPlayers(std::list<Player *> &list)
{
    int i, i2;
    int same;
    std::list<Player *>::iterator sorted;

    for (std::vector<Player *>::iterator it = m_players.begin();
         it != m_players.end(); ++it)
    {
        same = 0;
        i = *it->getInitiative();
        sorted = list.begin();
        while (sorted != m_players.end())
        {
            i2 = *sorted->getInitiative();
            if (i > i2)
            {
                list.insert(sorted, *it);
                break;
            }
            else if (i == i2)
            {
                ++same;
            }
            else if (same)
            {
                same = rollDie(same+1)-1;
                while (same)
                {
                    --sorted;
                    --same;
                }
                list.insert(sorted, *it);
                break;
            }
            ++sorted;
        }
        if (sorted == m_players.end())
            list.insert(sorted, *it);

    }
}
