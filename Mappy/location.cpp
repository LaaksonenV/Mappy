#include "location.h"

Location::Location(string id)
    : m_players(std::list<Player*>())
    , m_id(id)
{

}

Location::~Location()
{
    while (!m_players.empty())
        m_players.pop_back();
}

const Location::getId() const
{
    return m_id;
}

int Location::occupied() const
{
    return (int)m_players.size();
}

bool Location::moveIn(Player *p)
{
    m_players.push_back(p);
    if (m_players.size() > 1)
        return false;
    return true;
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
