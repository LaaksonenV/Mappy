#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <list>

class Location;
class Route;

class Player
{
public:
    Player(std::string name);
    ~Player();

    void setName(const std::string &name);
    const std::string getName() const;

    void setInitiative(int initiative);
    const int getInitiative() const;

//    void setLocation(Location *loc);
    Location *getLocation() const;

    void addMove(Location *move);
    std::list<Location*> getMoves() const;
    int step(bool bforth = true);
    void flipMove();
    void clearMoves();
    void clearAll();
    void clearMovesBehind();

private:
    std::string m_name;
    int m_initiative;

    bool m_bmoveBack;
    Route *m_first;
    Route *m_current;
};

#endif // PLAYER_H
