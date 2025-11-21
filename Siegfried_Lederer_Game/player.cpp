#include "player.h"

Player::Player(QGraphicsItem *parent)
    : GameObject(parent),
    m_speed(250.0), // velocidad horizontal aproximada
    m_lives(3)
{
}

void Player::setSpeed(double speed)
{
    m_speed = speed;
}

double Player::speed() const
{
    return m_speed;
}

void Player::setLives(int lives)
{
    if (m_lives == lives)
        return;

    m_lives = lives;
    emit livesChanged(m_lives);
}

int Player::lives() const
{
    return m_lives;
}

void Player::moveLeft(double dt)
{
    setPos(x() - m_speed * dt, y());
}

void Player::moveRight(double dt)
{
    setPos(x() + m_speed * dt, y());
}
