#include "player.h"


#include "location.h"

Player::Player(std::string name)
    : m_name(name)
    , m_initiative(0)

    , m_bmoveBack(false)
    , m_first(new Route(nullptr))

{
    m_current = m_first;
}

Player::~Player()
{
    m_current = nullptr;
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

void Player::setLocation(Location *loc)
{
    m_first->changeLocation(loc);
}

Location * Player::getLocation() const
{
    return m_current->location();
}

void Player::addMove(Location *move)
{
    m_first->addStep(move);
}

std::list<Location*> Player::getMoves() const
{
    return m_first->locations();
}

int Player::step(bool bforth)
{
    Route *nextLoc = m_current->next(m_bmoveBack != bforth);
    if (!nextLoc)
        return 0;

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

void Player::clearMove()
{
    m_current->clear();
    m_first = m_current;
    m_bmoveBack = false;
}

void Player::clearMovesBehind()
{
    if (m_bmoveBack)
        m_current->clearAfter();
    else
        m_current->clearBefore();

}
