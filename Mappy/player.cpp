#include "player.h"


#include "location.h"

Player::Player(std::string name)
    : m_name(name)
    , m_initiative(0)
    , m_currentLoc(nullptr)
    , m_move(std::list<Location*>())
{
    m_step = m_move.end();
}

Player::~Player()
{
    m_currentLoc = nullptr;
    clearMove();
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
    m_currentLoc = loc;
}

Location * Player::getLocation() const
{
    return m_currentLoc;
}

void Player::addMove(Location *move)
{
    m_move.push_back(move);
    m_step = m_move.begin();
}

const std::list<Location*> &Player::getMoves() const
{
    return m_move;
}

Location *Player::pullMove()
{
    if (m_step == m_move.end())
        return nullptr;
    Location *ret = *m_step;
    ++m_step;
    return ret;
}

void Player::flipMove()
{
    m_move.reverse();
    ++m_step;
}

void Player::clearMove()
{
    m_move.clear();
    m_step = m_move.end();
}
