#ifndef LOCATION_H
#define LOCATION_H

#include <list>

#include "player.h"


class Location
{
public:
    Location();
    ~Location();

    bool occupied() const;

    void moveIn(Player *p);
    bool moveOut(Player *p);

private:
    std::list<Player*> m_players;
};

#endif // LOCATION_H
