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
    Player *play = new Player(string("Player ")+std::to_string(ret));
    m_players.push_back(play);

    return ret;
}

string Campaign::getPlayerData(PlayerData id, int type)
{
    if (id >= m_players.size())
        return "";

    Player *play = m_players.at(id);
    std::string ret = "";
    switch (type)
    {
    case e_Initiative:
        ret = std::to_string(play->getInitiative());
        break;
    case e_Name:
        ret = play->getName();
        break;
    case e_Location:
        ret = play->getLocation()->getId();
        break;
    case e_Fight:
        ret = std::to_string(play->getLocation()->occupied()-1);
        break;
    case e_Moves:
        for (std::list<Location*>::iterator it = play->getMoves().begin();
             it < play->getMoves().end(); ++it)
        {
            ret += *it->getId();
            ret += "->";
        }
        if (ret != "")
            ret = ret.substr(0, (int)ret.size() -2);
        break;
    default:
        break;
    }
    return ret;
}

bool Campaign::setPlayerData(int id, int type, string data)
{
    if (id >= m_players.size())
        return false;

    Player *play = m_players.at(at);

    switch (type)
    {
    case e_Initiative:
        play->setInitiative(std::stoi(data));
        break;
    case e_Name:
        play->setName(data);
        break;
    case e_Location:
        play->setLocation(getLocation(data));
        break;
    case e_Moves:
        play->clearMove();
        size_t pos = data.find("->");
        size_t startpos = 0;
        while (pos != std::string::npos)
        {
            play->addMove(getLocation(data.substr(startpos, pos-startpos)));
            startpos = pos + 2;
        }
        break;
    default:
        break;
    }
    return true;
}

bool Campaign::startTurn()
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
    return true;
}

bool Campaign::endTurn()
{
    std::list<Player*> turn;
    sortPlayers(turn);

    std::list<Player*>::iterator it;
    Player *inTurn;
    Location *movement;

    bool ok = true;

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
                if (inTurn->getLocation()->occupied() > 1)
                    ok = false;
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
    return ok;
}

Location *Campaign::getLocation(std::string id)
{
    int xloc = 0;
    int yloc = -26;
    std::string::iterator it = id.begin();
    while (it != id.end())
    {
        if (int(*it) < 65 || int(*it) > 90)
            break;

        yloc += 26 += int(*it) - 65;

        ++it;
    }
    while (it != id.end())
    {
        if (int(*it) < 48 || int(*it) > 57)
            break;

        xloc *= 10;
        xloc += int(*it) - 48;

        ++it;
    }

    std::map<int, Location*> *y;
    Location *loc;
    if (m_map.count(yloc))
        y = m_map.at(yloc);
    else
    {
        char
        y = new std::map<int, Location*>();
        m_map.insert(yloc, y);
    }

    if (y->count(xloc))
        loc = y->at(xloc);
    else
    {
        loc = new Location(id);
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
