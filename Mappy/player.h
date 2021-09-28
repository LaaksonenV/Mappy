#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <list>

class Location;

class Player
{
public:
    Player(std::string name);
    ~Player();

    void setName(const std::string &name);
    const std::string getName() const;

    void setInitiative(int initiative);
    const int getInitiative() const;

    void setLocation(Location *loc);
    const Location *getLocation() const;

    void addMove(Location *move);
    const std::list<Location*> &getMoves() const;
    const Location *pullMove();
    void flipMove();
    void clearMove();

private:
    std::string m_name;
    int m_initiative;
    Location* m_currentLoc;
    std::list<Location*> m_move;
    std::list<Location*>::iterator m_step;

};

#endif // PLAYER_H
