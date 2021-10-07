#include "location.h"

Location::Location(std::string id)
    : m_players(std::list<Player*>())
    , m_id(id)
{

}

Location::~Location()
{
    while (!m_players.empty())
        m_players.pop_back();
}

const std::string Location::getId() const
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
    for (std::list<Player*>::iterator it = m_players.begin();
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

Route::Route(Location *loc, Route *prev)
{
    m_loc = loc;
    m_next = nullptr;
    m_prev = prev;
}

Route::~Route()
{
    m_loc = nullptr;
    m_prev = nullptr;
    if (m_next)
        delete m_next;
    m_next = nullptr;
}

Location *Route::location() const
{
    return m_loc;
}

std::list<Location*> Route::locations() const
{
    std::list<Location*> ret;
    ret.push_back(m_loc);
    if (m_next)
        ret.splice(ret.end(), m_next->locations());
    return ret;
}


void Route::changeLocation(Location *loc)
{
    m_loc = loc;
}

void Route::addStep(Location *loc)
{
    if (m_next)
        m_next->addStep(loc);
    else
        m_next = new Route(loc, this);
}

Route *Route::next(bool forward)
{
    if (forward)
        return m_next;
    else
        return m_prev;
}

void Route::clear()
{
    clearAfter();
    clearBefore();
}

void Route::clearAfter()
{
    if (m_next)
        delete m_next;
    m_next = nullptr;
}

void Route::clearBefore()
{
    if (m_prev)
    {
        m_prev->m_next = nullptr;
        m_prev->clearBefore();
        delete m_prev;
        m_prev = nullptr;
    }
}
