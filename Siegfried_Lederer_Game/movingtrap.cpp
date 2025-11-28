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
    m_t = 0.0;               // reiniciamos tiempo
    setPos(m_center);        // posición inicial
}

void MovingTrap::setAmplitude(double amplitude)
{
    m_amplitude = amplitude; // radio del círculo
}

void MovingTrap::setOmega(double omega)
{
    m_omega = omega;
}

void MovingTrap::update(double dt)
{
    m_t += dt;

    // Trayectoria circular en torno a m_center:
    // x(t) = cx + R cos(ωt)
    // y(t) = cy + R sin(ωt)
    double offsetX = m_amplitude * qCos(m_omega * m_t);
    double offsetY = m_amplitude * qSin(m_omega * m_t);

    setPos(m_center.x() + offsetX,
           m_center.y() + offsetY);
}
