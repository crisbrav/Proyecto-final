#include "tankprojectile.h"

TankProjectile::TankProjectile(double vx,
                               double vy,
                               double gravity,
                               QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent),
    m_vx(vx),
    m_vy(vy),
    m_gravity(gravity)
{
}

void TankProjectile::setVelocity(double vx, double vy)
{
    m_vx = vx;
    m_vy = vy;
}

void TankProjectile::setGravity(double g)
{
    m_gravity = g;
}

void TankProjectile::update(double dt)
{
    // Integración sencilla:
    // y(t+dt) = y(t) + vy*dt + 0.5*g*dt^2
    // vy      = vy + g*dt
    //
    // Recuerda: en Qt el eje Y crece hacia abajo,
    // así que:
    //  - vy negativo = hacia arriba
    //  - gravedad positiva = aceleración hacia abajo

    m_vy += m_gravity * dt;

    double dx = m_vx * dt;
    double dy = m_vy * dt;

    setPos(x() + dx, y() + dy);
}
