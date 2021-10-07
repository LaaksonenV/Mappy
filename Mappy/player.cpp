#include "player.h"


#include "location.h"

Player::Player(std::string name)
    : m_name(name)
    , m_initiative(0)

    , m_bmoveBack(false)
    , m_first(nullptr)

{
    m_current = m_first;
}

Player::~Player()
{
    m_current = nullptr;
    if (m_first)
        delete m_first;
    m_first = nullptr;
}

void Player::setName(const std::string &name)
{
    m_name = name;
}

const std::string Player::getName() const
{
    return m_name;
}

void Player::setInitiative(int initiative)
{
    m_initiative = initiative;
}

const int Player::getInitiative() const
{
    return m_initiative;
}

/*void Player::setLocation(Location *loc)
{
    if (!m_current)
    {
        m_first = new Route(loc);
        m_current = m_first;
    }
    m_current->changeLocation(loc);
}*/

Location * Player::getLocation() const
{
    if (!m_current)
        return nullptr;
    return m_current->location();
}

bool Player::retreating() const
{
    return m_bmoveBack;
}

void Player::addMove(Location *move)
{
    if (!m_first)
    {
        m_first = new Route(move);
        m_current = m_first;
        if (move)
            move->moveIn(this);
    }
    else
        m_first->addStep(move);
}

std::list<Location*> Player::getMoves() const
{
    if (!m_first)
        return std::list<Location*>();

    return m_first->locations();
}

int Player::step(bool bforth)
{
    if (!m_current)
        return -1;
    Route *nextLoc = m_current->next(m_bmoveBack != bforth);
    if (!nextLoc)
        return -1;

    if (m_current->location())
        m_current->location()->moveOut(this);
    m_current = nextLoc;
    if (m_current->location())
    {
        m_current->location()->moveIn(this);
        return m_current->location()->occupied();
    }
    return 0;
}

void Player::flipMove()
{
    m_bmoveBack = !m_bmoveBack;
}

void Player::clearMoves()
{
    if (m_current)
    {
        m_current->clear();
        m_first = m_current;
    }
    m_bmoveBack = false;
}

void Player::clearAll()
{
    if (m_current && m_current->location())
        m_current->location()->moveOut(this);
    if (m_first)
    {
        delete m_first;
        m_first = nullptr;
        m_current = nullptr;
    }
    m_bmoveBack = false;
}

void Player::clearMovesBehind()
{
    if (m_current)
    {
        if (m_bmoveBack)
            m_current->clearAfter();
        else
            m_current->clearBefore();
    }
}
