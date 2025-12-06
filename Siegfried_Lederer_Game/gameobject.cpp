#include "gameobject.h"

GameObject::GameObject(QGraphicsItem *parent)
    : QObject(),
    QGraphicsPixmapItem(parent),
    m_velocity(0.0, 0.0)
{
}

void GameObject::setVelocity(const QPointF &v)
{
    m_velocity = v;
}

QPointF GameObject::velocity() const
{
    return m_velocity;
}

void GameObject::update(double dt)
{
    // comportamiento por defecto: mover según velocidad lineal
    if (!m_velocity.isNull()) {
        setPos(pos() + m_velocity * dt);
    }
}
