#include "campaign.h"

#include <stdlib.h>
#include <time.h>
//#include <map>
//#include <string>
#include <vector>

#include "player.h"
#include "location.h"

Campaign::Campaign()
    : m_players(std::vector<Player*>(0))
    , m_map(std::map<int, std::map<int, Location*> *>())
{
    srand(time(NULL));
}

Campaign::~Campaign()
{
    for (Player *p : m_players)
    {
        delete p;
        p = nullptr;
    }
    for (std::map<int, std::map<int, Location*> *>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        for (std::map<int, Location*>::iterator itit = it->second->begin();
             itit != it->second->end(); ++itit)
        {
            delete itit->second;
        }
        delete it->second;
    }
}

int Campaign::rollDie(int d)
{
    return rand() % d + 1;
}

int Campaign::addPlayer()
{
    int ret = (int)m_players.size();
    Player *play = new Player(std::string("Player ")+std::to_string(ret+1));
    m_players.push_back(play);

    return ret;
}

std::string Campaign::getPlayerData(int id, int type)
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
        if (play->getLocation())
            ret = play->getLocation()->getId();
        else
            ret = "";
        break;
    case e_Fight:
        if (play->getLocation() && play->getLocation()->occupied() > 1)
            ret = "1";
        else
            ret = "0";
        break;
    case e_Moves:
        for (std::list<Location*>::const_iterator it = play->getMoves().begin();
             it != play->getMoves().end(); ++it)
        {
            ret += (*it)->getId();
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

bool Campaign::setPlayerData(int id, int type, std::string data)
{
    if (id >= m_players.size())
        return false;


    Player *play = m_players.at(id);
    size_t pos = 0;
    size_t startpos = 0;

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
        pos = data.find("->");
        while (pos != std::string::npos)
        {
            play->addMove(getLocation(data.substr(startpos, pos-startpos)));
            startpos = pos + 2;
            pos = data.find("->", startpos);
        }
        play->addMove(getLocation(data.substr(startpos)));
        break;
    case e_Fight:
        if (std::stoi(data) < 0)
            play->flipMove();
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
    int occupation;

    while (!turn.empty())
    {
        it = turn.begin();
        while (it != turn.end())
        {
            inTurn = *it;
            occupation = inTurn->step();
            if (occupation)
            {
                if (occupation > 1)
                    it = turn.erase(it); // battle, pause movement
                else
                    ++it;
            }
            else
                it = turn.erase(it); // no more moves

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
    int occupation;

    bool ok = true;
    int turns = 0;
    int steps = 0;

    while (!turn.empty())
    {
        it = turn.begin();
        ++turns;
        while (it != turn.end())
        {
            inTurn = *it;
            occupation = inTurn->step();
            if (!occupation) // no more moves or stepped out
            {
                it = turn.erase(it);
            }
            else if (occupation > 1) // move blocked (could wait)
            {
                steps = 0;
                while (inTurn->step(false) > 1 && steps < turns)
                { ++ steps; } // move back up to where restarted moving
                it = turn.erase(it);
                if (inTurn->getLocation()->occupied() > 1)
                    ok = false;
            }
            else // allowed move, moving on
            {
                ++it;
            }
        }
    }
    if (ok)
        for (Player *p : m_players)
            p->clearMove();
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

        yloc += 26;
        yloc += int(*it) - 65;

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
        y = new std::map<int, Location*>();
        m_map[yloc] = y;
    }

    if (y->count(xloc))
        loc = y->at(xloc);
    else
    {
        loc = new Location(id);
        (*y)[xloc] = loc;
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
        i = (*it)->getInitiative();
        sorted = list.begin();
        while (sorted != list.end())
        {
            i2 = (*sorted)->getInitiative();
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
        if (sorted == list.end())
            list.insert(sorted, *it);

    }
}
