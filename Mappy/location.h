#ifndef LOCATION_H
#define LOCATION_H

#include <list>

#include "player.h"


class Location
{
public:
    Location(std::string id);
    ~Location();

    const std::string getId() const;

    int occupied() const;

    bool moveIn(Player *p);
    bool moveOut(Player *p);

private:
    std::list<Player*> m_players;
    std::string m_id;
};

#endif // LOCATION_H
