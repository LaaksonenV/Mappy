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
        if (p)
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

bool Campaign::removePlayer(int id)
{
    Player *play = getPlayer(id);
    if (!play)
        return false;

    delete play;
    m_players[id] = nullptr;
    return true;
}

std::string Campaign::getPlayerData(int id, int type)
{
    Player *play = getPlayer(id);
    if (!play)
        return "";

    std::string ret = "";
    std::list<Location*> moves;
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
        moves = play->getMoves();
        for (std::list<Location*>::iterator it = moves.begin();
             it != moves.end(); ++it)
        {
            if (!(*it))
                ret += "OffMap";
            else
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

bool Campaign::setPlayerData(int id, int type, const std::string &data)
{

    Player *play = getPlayer(id);
    if (!play)
        return false;

    size_t pos = 0;
    size_t startpos = 0;
    std::string text;

    switch (type)
    {
    case e_Initiative:
        play->setInitiative(std::stoi(data));
        break;
    case e_Name:
        play->setName(data);
        break;
    case e_Location:
        play->clearAll();
        play->addMove(getLocation(data));
        break;
    case e_Moves:
        play->clearAll();
        pos = data.find("->");
        while (pos != std::string::npos)
        {
            text = data.substr(startpos, pos-startpos);
            if (text.back() == '*')
            {
                text.pop_back();
                play->addMove(getLocation(text));
                while (play->step() >= 0)
                {}
            }
            else
                play->addMove(getLocation(text));
            startpos = pos + 2;
            pos = data.find("->", startpos);
        }
        if (!data.empty())
        {
            text = data.substr(startpos);
            if (text.back() == '*')
            {
                text.pop_back();
                play->addMove(getLocation(text));
                while (play->step() >= 0)
                {}
            }
            else
                play->addMove(getLocation(text));
        }
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

    // Each player moves one step in their turn, until every player has made
    // all their moves, or landed on a spot with other players.

    // turn is a list containing all players still moving in predetermined order
    while (!turn.empty()) // round starts
    {
        // iterator it goes through remaining players each round
        it = turn.begin();
        while (it != turn.end()) // one round
        {
            inTurn = *it;

            if (inTurn->getLocation() && inTurn->getLocation()->occupied() > 1)
                it = turn.erase(it); // battle in progress, pause movement
            else // start a move
            {
                // step() moves the player and returns the amount of players in
                // the location it moved to, 0 if offmap, -1 if out of moves
                occupation = inTurn->step();
                if (occupation >= 0) // took a step
                {
                    if (occupation > 1)
                        it = turn.erase(it); // battle initiated, pause movement
                    else
                        ++it; // took a step to unoccupied location, next player
                }
                else
                    it = turn.erase(it); // no more moves
            }
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

    bool movement = true; // has someone moved
//    int turns = 0;
    int steps = 0;

    // Each player whose move was interrupted by a battle will make moves now,
    // winners finishing their moves and losers retreating back to an unoccupied
    // location. If the retreating player can't reach an unoccupied location,
    // the GM will have to manually place the player to an agreeable location.
    // Winning players will move normally, though not stepping on an occupied
    // location, instead trying again next round. Losing players will move once
    // back up to where they started, moving over occupied locations. If they
    // cannot move, they try again next round. Turn ends when all players have
    // finished their moves or no one is able to move during a round.

    while (!turn.empty() && movement)
    {
        it = turn.begin();
        movement = false;
//        ++turns;
        while (it != turn.end())
        {
            inTurn = *it;
            occupation = inTurn->step();
            if (occupation <= 0) // no more moves or stepped off the map
            {
                movement = true;
                it = turn.erase(it);
            }
            else if (inTurn->retreating()) // keep moving if can/needed
            {
                if (occupation < 2) // retreated to a valid location
                {
                    movement = true;
                    it = turn.erase(it);
                }
                else // (occupation > 1) keep trying to move
                {
                    steps = 1;
                    while ((occupation = inTurn->step()) > 1)
                        ++steps;
                    if (occupation == 0
                            || inTurn->getLocation()->occupied() == 1)
                        // valid location (or offmap)
                    {
                        movement = true;
                        it = turn.erase(it);
                    }
                    else // out of moves with no valid location
                    {
                        while (steps) // walk back, try again next round
                        {
                            inTurn->step(false);
                            --steps;
                        }
                        ++it;
                    }
                }
            }
            else if (occupation > 1) // move blocked, try again next round
            {
                ++it;
                inTurn->step(false);
            }
            else // allowed move, moving on
            {
                movement = true;
                ++it;
            }
        }
    }
    bool ok = true;
    for (Player *p : m_players) // check that all players are free
    {
        if (p && p->getLocation() && p->getLocation()->occupied() > 1)
        {
            ok = false;
            break;
        }
    }
    if (ok)
        for (Player *p : m_players)
            if (p)
                p->clearMoves();
    return ok;
}

Location *Campaign::getLocation(const std::string &id)
{
    int xloc, yloc;

    if (!getCoords(id, xloc, yloc))
        return nullptr;

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

int Campaign::checkMoves(const std::string &moves)
{
    size_t startpos = 0;
    size_t pos = moves.find("->");
    std::string text;
    int xloc = -1, yloc = -1;
    int xnext, ynext;
    int move = 0;

    while (pos != std::string::npos)
    {
        text = moves.substr(startpos, pos-startpos);
        if (text.back() == '*')
            text.pop_back();

        if (!getCoords(text, xnext, ynext))
            return move;

        if (xloc > -1 && !checkCoords(xloc, yloc, xnext, ynext))
            return move;

        xloc = xnext;
        yloc = ynext;
        ++move;

        startpos = pos + 2;
        pos = moves.find("->", startpos);
    }
    if (!moves.empty())
    {
        text = moves.substr(startpos);
        if (text.back() == '*')
            text.pop_back();

        if (!getCoords(text, xnext, ynext))
            return move;

        if (xloc > -1 && !checkCoords(xloc, yloc, xnext, ynext))
            return move;
    }
    return -1;
}

void Campaign::sortPlayers(std::list<Player *> &list)
{
    int i, i2;
    int same;
    std::list<Player *>::iterator sorted;

    for (std::vector<Player *>::iterator it = m_players.begin();
         it != m_players.end(); ++it)
    {
        if (*it)
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
}

Player* Campaign::getPlayer(int at)
{
    if (at >= m_players.size())
        return nullptr;

    Player *play = m_players.at(at);

    return play;
}

bool Campaign::getCoords(const std::string &id, int &xloc, int &yloc)
{
    xloc = 0;
    yloc = -26;
    std::string::const_iterator it = id.begin();
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

    if (xloc < 0 || yloc < 0)
        return false;
    return true;
}

bool Campaign::checkCoords(int xstart, int ystart, int xend, int yend)
{
    if (ystart == yend)
    {
        if (abs(xstart - xend) == 1)
            return true;
        return false;
    }
    else if (abs(ystart - yend) == 1)
    {
        // paired row can end 0 and 1 col diff, unpaired 0 and -1
        int xdiff = xend - xstart + ystart % 2;
        if (xdiff == 0 || xdiff == 1)
            return true;
        return false;
    }
    return false;
}
