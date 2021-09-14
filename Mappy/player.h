#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <queue>

class Location;

class Player
{
public:
    Player(std::string name);
    ~Player();

    const std::string getName() const;

    void setInitiative(int initiative);
    const int getInitiative() const;

    void setLocation(Location *loc);
    const Location *getLocation() const;

    void addMove(Location *move);
    const Location *pullMove();
    void clearMove();

private:
    std::string m_name;
    int m_initiative;
    Location* m_currentLoc;
    std::queue<Location*> m_move;

};

#endif // PLAYER_H