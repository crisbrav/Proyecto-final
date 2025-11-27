#include "movingtrap.h"
#include <QtMath>

MovingTrap::MovingTrap(QGraphicsItem *parent)
    : GameObject(parent),
    m_center(0.0, 0.0),
    m_amplitude(40.0),
    m_omega(2.0),
    m_t(0.0)
{
}

void MovingTrap::setCenter(const QPointF &center)
{
    m_center = center;
    m_t = 0.0;        // reinicia el tiempo
    setPos(m_center);
}


void MovingTrap::setAmplitude(double amplitude)
{
    m_amplitude = amplitude;
}

void MovingTrap::setOmega(double omega)
{
    m_omega = omega;
}

void MovingTrap::update(double dt)
{
    m_t += dt;
    double offset = m_amplitude * qSin(m_omega * m_t);
    // movimiento senoidal VERTICAL
    setPos(m_center.x(), m_center.y() + offset);
}

