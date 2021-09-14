#include "campaign.h"

#include <stdlib.h>
#include <time.h>
#include <map>

#include "player.h"
#include "location.h"

Campaign::Campaign()
    : m_players(std::list<Player*>())
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

Player *Campaign::addPlayer(std::string name, int initiative,
                            int xloc, int yloc)
{
    Player *play = new Player(name);
    play->setInitiative(initiative);


    play->setLocation(getLocation(xloc, yloc));

    return play;
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
            if (movement)
            {
                if (movement->occupied())
                {
                    turn.erase(it); // occupied, stop movement
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
            else
                turn.erase(it); // no more moves

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

