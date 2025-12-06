#include "bomb.h"

Bomb::Bomb(QGraphicsItem *parent)
    : GameObject(parent),
    m_gravity(600.0),   // valor de ejemplo
    m_verticalSpeed(0.0)
{
}

void Bomb::setGravity(double g)
{
    m_gravity = g;
}

double Bomb::gravity() const
{
    return m_gravity;
}

void Bomb::update(double dt)
{
    // MRUA: v = v0 + a * t ; y = y + v * dt
    m_verticalSpeed += m_gravity * dt;
    setPos(x(), y() + m_verticalSpeed * dt);
}
