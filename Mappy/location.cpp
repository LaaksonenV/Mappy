#include "location.h"

Location::Location()
    : m_players(std::list<Player*>())
{

}

Location::~Location()
{
    while (!m_players.empty())
        m_players.pop_back();
}

bool Location::occupied() const
{
    return !m_players.empty();
}

void Location::moveIn(Player *p)
{
    m_players.push_back(p);
}

bool Location::moveOut(Player *p)
{
    for (std::list::iterator<Player*> it = m_players.begin();
         it != m_players.end(); ++it)
    {
        if (*it = p)
        {
            m_players.erase(it);
            return true;
        }
    }
    return false;
}
