// projectile.cpp
#include "projectile.h"

Projectile::Projectile(QGraphicsItem *parent)
    : GameObject(parent),
    m_acceleration(0.0, 0.0)
{
}

void Projectile::setAcceleration(const QPointF &a)
{
    m_acceleration = a;
}

QPointF Projectile::acceleration() const
{
    return m_acceleration;
}

void Projectile::update(double dt)
{
    // v = v0 + a * t
    m_velocity += m_acceleration * dt;
    setPos(pos() + m_velocity * dt);
}
