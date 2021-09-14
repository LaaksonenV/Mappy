#include "player.h"


#include "location.h"

Player::Player(std::string name)
    : m_name(name)
    , m_initiative(0)
    , m_currentLoc(nullptr)
    , m_move(std::queue<Location*>())
{

}

Player::~Player()
{
    m_currentLoc = nullptr;
    clearMove();
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

const Location * Player::getLocation() const
{
    return m_currentLoc;
}

void Player::addMove(Location *move)
{
    m_move.push(move);
}

const Location * Player::pullMove()
{
    if (m_move.empty())
        return nullptr;
    Location *ret = m_move.front();
    m_move.pop();
    return ret;
}

void Player::clearMove()
{
    while (!m_move.empty())
        m_move.pop();
}
