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


class Route
{
public:
    Route(Location *loc, Route *prev = nullptr);
    ~Route();

    Location *location() const;
    std::list<Location*> locations() const;
    void changeLocation(Location *loc);
    void addStep(Location *loc);
    Route *next(bool forward);

    void clear();
    void clearAfter();
    void clearBefore();

private:
    Location *m_loc;
    Route *m_next;
    Route *m_prev;
};

#endif // LOCATION_H
